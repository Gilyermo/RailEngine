#include "ThreadManager.h"
#include "..\Memory\PoolAllocator.h"
#include "..\Memory\Adapter.h"
#include <algorithm>


namespace RGE
{
namespace Multithreading
{

Memory::PoolAllocator<Thread>		g_threadPoolAllocator(10);


ThreadManager::ThreadManager() : 
	m_minWorkersCount(1),
	m_maxTaskCount(5),
	m_sleepTime(20)
{
//HACK: эти две строчки позволяют избежать deadlock при уничтожении менеджера под Windows
	Thread th;
	th.Join();
	
	uint8_t workersCount = std::max<int8_t>( std::thread::hardware_concurrency()-1, m_minWorkersCount );
	m_maxThreadCount = 4 * workersCount;

	m_workers.resize(workersCount);
	for(auto& worker : m_workers.get_unsafe())
		worker = Memory::CreateShared<Thread>(g_threadPoolAllocator);

	m_makeBalancing.store(false);
	m_masterIsFree.store(true);
	m_masterTask = std::make_shared<Task<void>>( [this](){ MasterJob(); } );		//TODO: создавать таски через распределители памяти
	m_masterTask->SetState(TS_READY);
}

ThreadManager::~ThreadManager(){
	for(auto& condemned : m_condemnedThreads.get_unsafe())	condemned->Join();
	for(auto& lentThread : m_lentThreads.get_unsafe())		lentThread->Join();
	for(auto& worker : m_workers.get_unsafe())				worker->Join();
	m_master.Join();
}


ThreadManager& ThreadManager::Instance(){
	static ThreadManager singleton;
	return singleton;
}


void ThreadManager::WaitAllTasksCompleted() const{
	m_masterTask->Wait();

	while(true){
		ThreadPtr busyWorker = FindMostBusyWorker();
		if(busyWorker->IsFree())
			return;

		while(!busyWorker->IsFree())
			std::this_thread::sleep_for(m_sleepTime);
	}
}

bool ThreadManager::AreAllTasksCompleted() const{
	return FindMostBusyWorker()->IsFree();
}

int ThreadManager::GetQueuedTaskCount() const{
	return	m_tasks[TP_MINIMAL].size() + 
			m_tasks[TP_NORMAL].size() +
			m_tasks[TP_MAXIMAL].size();
}


void ThreadManager::SetSleepTime(long milliseconds){
	m_sleepTime = std::chrono::milliseconds( milliseconds );
}

bool ThreadManager::SetMaxThreadCount(uint8_t maxThreadCount){
	int16_t count = maxThreadCount - GetThreadsCount();
	if(count<0){
		uint8_t realCount = SetWorkersCount( static_cast<uint8_t>(-count) );
		if(realCount != -count)
			return false;
	}

	m_maxThreadCount = maxThreadCount;
	return true;
}

uint8_t ThreadManager::GetMaxThreadCount() const{
	return m_maxThreadCount;
}

void ThreadManager::SetMaxTaskCount(uint8_t maxTaskCount){
	m_maxTaskCount = maxTaskCount;
}

uint8_t ThreadManager::GetMaxTaskCount() const{
	return m_maxTaskCount;
}

float ThreadManager::GetLoadPercent() const{
	float load = 0.0f;
	
	m_workers.lock();
	for(auto& worker : m_workers.get_unsafe())
		load += worker->GetTaskCount();
	m_workers.unlock();

	load = load / (m_maxTaskCount*m_workers.size()) * 100.0f;
	return load;
}


uint8_t ThreadManager::GetThreadsCount() const{
	return static_cast<uint8_t>( m_workers.size() + m_lentThreads.size() + 1 );
}

uint8_t ThreadManager::GetWorkersCount() const{
	return static_cast<uint8_t>( m_workers.size() );
}

uint8_t ThreadManager::GetLentThreadsCount() const{
	return static_cast<uint8_t>( m_lentThreads.size() );
}

uint8_t ThreadManager::GetCondemnedThreadsCount() const{
	return static_cast<uint8_t>( m_condemnedThreads.size() );
}

uint8_t ThreadManager::SetWorkersCount(uint8_t _count){
	int16_t count = _count - GetWorkersCount();
	
	if(count > 0)		IncreaseWorkersCount( static_cast<uint8_t>(count) );
	else if(count < 0)	DecreaseWorkersCount( static_cast<uint8_t>(-count) );
	return GetWorkersCount();
}

uint8_t ThreadManager::IncreaseWorkersCount(uint8_t _count){
	uint8_t count = std::min<uint8_t>( _count, m_maxThreadCount - GetThreadsCount() );
	ThreadPtr th;

	for(uint8_t i=0; i<count; i++){
		if(!m_condemnedThreads.try_get_and_pop_back(&th))
			th = Memory::CreateShared<Thread>(g_threadPoolAllocator);
		m_workers.push_back(th);
	}

	return count;
}

uint8_t ThreadManager::DecreaseWorkersCount(uint8_t _count){
	uint8_t count = std::min<uint8_t>(_count, m_workers.size() - m_minWorkersCount);

	for(uint8_t i=0; i<count; i++){
		ThreadPtr th = ReleaseWorkerThread();
		m_condemnedThreads.push_back(th);
	}

	if(!m_tasks[TP_MAXIMAL].empty())
		LaunchMasterThread();

	return count;
}


ThreadPtr ThreadManager::LendThread(bool createNew){
	ThreadPtr thread;

	if(!m_condemnedThreads.try_get_and_pop_back(&thread)){
		if(createNew && GetThreadsCount() < m_maxThreadCount){
			thread = Memory::CreateShared<Thread>(g_threadPoolAllocator);
		}
		else if(GetWorkersCount() > m_minWorkersCount){
			thread = ReleaseWorkerThread();
			if(!m_tasks[TP_MAXIMAL].empty())
				LaunchMasterThread();
		}
	}

	if(thread != nullptr)	
		m_lentThreads.push_back(thread);
	return thread;
}

void ThreadManager::ReturnThread(ThreadPtr& th){
	if(m_lentThreads.try_erase(th))
		m_workers.push_back(th);
	th.reset();
} 


void ThreadManager::SetMakeBalancing(bool balancing){
	m_makeBalancing.store(balancing);
}


ThreadPtr ThreadManager::FindMostFreeWorker() const{
	if(m_workers.size()==1)		
		return m_workers.front();

	m_workers.lock();
	ThreadPtr mostFreeWorker = m_workers.get_unsafe().front();
	for(auto& worker : m_workers.get_unsafe())
		if(mostFreeWorker->GetTaskCount() > worker->GetTaskCount() || !mostFreeWorker->IsFree() && worker->IsFree())
			mostFreeWorker = worker;
	m_workers.unlock();

	return mostFreeWorker;
}

ThreadPtr ThreadManager::FindMostBusyWorker() const{
	if(m_workers.size()==1)		
		return m_workers.front();

	m_workers.lock();
	ThreadPtr mostBusyWorker = *std::max_element(	m_workers.get_unsafe().begin(),
													m_workers.get_unsafe().end(),
													[&](const ThreadPtr& worker, const ThreadPtr& busyWorker){
														return busyWorker->GetTaskCount() > worker->GetTaskCount();
													}
												 );
	m_workers.unlock();
	return mostBusyWorker;
}

bool ThreadManager::TryGetNextTask(ITaskPtr& task){
	if(m_tasks[TP_MAXIMAL].try_get_and_pop_front(&task))	return true;
	if(m_tasks[TP_NORMAL].try_get_and_pop_front(&task))		return true;
	if(m_tasks[TP_MINIMAL].try_get_and_pop_front(&task))	return true;

	return false;
}


void ThreadManager::TaskAssignement(){
	ThreadPtr freeWorker;
	ITaskPtr  task;

	while(TryGetNextTask(task)){
		freeWorker = FindMostFreeWorker();

		while(freeWorker->GetTaskCount() >= m_maxTaskCount){
			std::this_thread::sleep_for(m_sleepTime);
			freeWorker = FindMostFreeWorker();
		}

		freeWorker->Perform(task);
		m_hasNewTask.store(false);
	}
}

void ThreadManager::ThreadBalancing(){
	ThreadPtr freeWorker, busyWorker;
	ITaskPtr  task;

	if(m_workers.size() == 1)
		return;

	busyWorker = FindMostBusyWorker();
	while(busyWorker->GetTaskCount() > 0){
		freeWorker = FindMostFreeWorker();

		if(busyWorker->GetTaskCount() < freeWorker->GetTaskCount() + 1)
			std::this_thread::sleep_for(m_sleepTime);
		else
			if(busyWorker->TryGetLastTask(task))
				freeWorker->Perform(task);

		if(m_hasNewTask) 
			TaskAssignement();

		busyWorker = FindMostBusyWorker();
	}
}

void ThreadManager::JoinCondemnedThreads(){
	ThreadPtr th;
	while(m_condemnedThreads.try_get_and_pop_front(&th))
		th->Join();
}

void ThreadManager::MasterJob(){
	while(m_hasNewTask){
		m_masterIsFree.store(false);

		TaskAssignement();
		if(m_makeBalancing)		ThreadBalancing();
		JoinCondemnedThreads();
	
		m_masterIsFree.store(true);
	}
}

void ThreadManager::LaunchMasterThread(){
	m_hasNewTask.store(true);
	if(m_masterIsFree){
		m_masterTask->SetState(TS_QUEUED);
		m_master.Perform(m_masterTask);
	}
}


ThreadPtr ThreadManager::ReleaseWorkerThread(){
	m_workers.lock();
	auto freeWorker = std::max_element(	m_workers.get_unsafe().begin(),
										m_workers.get_unsafe().end(),
										[&](const ThreadPtr& freeWorker, const ThreadPtr& worker){
											return freeWorker->GetTaskCount() > worker->GetTaskCount();
										}
									   );
	ThreadPtr mostFreeWorker = *freeWorker;
	m_workers.get_unsafe().erase(freeWorker);
	m_workers.unlock();

	GrabAllTasks(mostFreeWorker);

	return mostFreeWorker;
}

void ThreadManager::GrabAllTasks(ThreadPtr& th){
	th->Suspend();
	ITaskPtr task;
	while(th->TryGetLastTask(task))
		m_tasks[TP_MAXIMAL].push_front(task);
	th->Resume();
}

}
}
