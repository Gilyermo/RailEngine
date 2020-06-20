#include "Thread.h"


namespace RGE
{
namespace Multithreading
{

std::atomic<uint16_t>	Thread::m_totalThreadCount = 0;

Thread::Thread(){
	m_enable.store(true);
	m_active.store(true);
	m_free.store(true);
	m_thread = std::move( std::thread([this](){ThreadFunction();}) );

	m_localID = m_totalThreadCount++;
}

Thread::~Thread(){
	Join();
}


void Thread::Perform(const ITaskPtr& task){
	m_tasks.push_back(task);
	std::lock_guard<std::mutex> lock(m_mutex);
	m_cond.notify_one();
}

std::shared_ptr<Task<void>> Thread::Execute(std::function<void()> f){
	auto newTask = std::make_shared<Task<void>>(f);		//TODO: создавать таски через распределители памяти

	m_tasks.push_back( newTask );
	std::lock_guard<std::mutex> lock(m_mutex);
	m_cond.notify_one();

	return newTask;
}

bool Thread::TryGetLastTask(ITaskPtr& task){
	return m_tasks.try_get_and_pop_back(&task);
}

size_t Thread::GetTaskCount() const{
	return m_tasks.size();
}


void Thread::Join(){
	if(!IsAlive())	return;

	m_enable.store(false);
	m_active.store(true);			// делаем поток активным, чтобы он мог выйти из основго цикла
	try{
		m_mutex.lock();
		m_cond.notify_one();
		m_mutex.unlock();
		m_thread.join();
		m_active.store(false);
	}
	catch(std::exception&){
		//TODO: записать исключение в лог...когда-нибудь
	}
}

void Thread::Detach(){
	if(!IsAlive())	return;

	m_enable.store(false);
	m_active.store(true);			// делаем поток активным, чтобы он мог выйти из основго цикла
	try{
		m_mutex.lock();
		m_cond.notify_one();
		m_mutex.unlock();
		m_thread.detach();
		m_active.store(false);
	}
	catch(std::exception&){
		//TODO: записать исключение в лог...когда-нибудь
	}
}

// поток уснет только после выполнения текущей задачи
void Thread::Suspend(){
	m_active.store(false);
}

void Thread::Resume(){
	m_active.store(true);
	std::lock_guard<std::mutex> lock(m_mutex);
	m_cond.notify_one();
}


bool Thread::IsFree() const{
	return m_free;
}

bool Thread::IsActive() const{
	return m_active;
}

bool Thread::IsAlive() const{
	return m_enable && m_thread.joinable();
}

uint16_t Thread::LocalID() const{
	return m_localID;
}


void Thread::ThreadFunction(){
	ITaskPtr task;

	while(m_enable){
		m_free.store(true);

		std::unique_lock<std::mutex> lock(m_mutex);
		m_cond.wait( lock, [&](){return !m_tasks.empty() && m_active || !m_enable;} );
		lock.unlock();

		m_free.store(false);
		while(m_enable && m_tasks.try_get_and_pop_front(&task)){
			task->Perform();

			if(!m_active){
				lock.lock();
				m_cond.wait( lock, [&](){return m_active || !m_enable;} );
				lock.unlock();
			}
		}
	}
}

}
}
