/************************************************************
*				����� ��� ������ � ��������;				*
*	������� ��� ������� � ������������ �������� ����� ����;	*
************************************************************/
#pragma once

#include "Multithreading.h"
#include "Thread.h"
#include "Task.h"
#include "Multitask.h"
#include <list>
#include <memory>


namespace RGE
{
namespace Multithreading
{
	
typedef std::pair< size_t, const char* >	ThreadID;


class KERNEL_API ThreadManager{
public:	
	typedef SafeContainer< std::deque<ITaskPtr> >	TaskDeque;
	typedef SafeContainer< std::list<ThreadPtr> >	ThreadList;

private:
	ThreadManager();
	ThreadManager(const ThreadManager&) = delete;
	ThreadManager(ThreadManager&&) = delete;

	ThreadManager& operator=(const ThreadManager&) = delete;

public:
	~ThreadManager();

	static ThreadManager& Instance();


	template <typename T>
	TaskProxy<T> Execute(std::function<T()> f, TaskPriority priority=TP_NORMAL){
		auto newTask = std::make_shared<Task<T>>(f);		//TODO: ��������� ����� ����� ���. ��� �� ������ ���� ���������?

		newTask->SetPriority(priority);
		m_tasks[priority].push_back(newTask);
		LaunchMasterThread();
		
		return TaskProxy<T>(newTask);
	}

	template <typename T>
	MultitaskProxy<T> Execute(std::function<T(int rank, int size)> f, uint8_t numThreads){
		auto newTask = std::make_shared<Multitask<T>>(f, numThreads);		//TODO: ��������� ����� ����� ���. ��� �� ������ ���� ���������?
		for(int i=0; i<newTask->GetTaskCount(); i++){
			auto subtask = newTask->GetSubtask(i);
			subtask->SetPriority(TP_MAXIMAL);
			m_tasks[TP_MAXIMAL].push_back(subtask);
		}
		LaunchMasterThread();

		return MultitaskProxy<T>(newTask);
	}
	
	template <typename T>
	bool RerunTask(TaskProxy<T>& task){
		if(task.GetState() <= TS_PROCESSING)	// ������� ��� �� �����������
			return false;

		task.SetState(TS_QUEUED);
		m_tasks[task.GetPriority()].push_back(task.m_task);
		LaunchMasterThread();

		return true;
	}

	template <typename T>
	bool ChangeTaskPriority(TaskProxy<T>& task, TaskPriority newPriority){
		if(task.GetPriority() == newPriority)	return true;
		if(task.GetState() != TS_QUEUED)		return false;	// ������� � �������� ���������� ��� ��������

	// ������ ����� ������� ��������� �� ��������� �������, �� ��� ��������� � ��� �������
		if(!m_tasks[task.GetPriority()].try_erase(task.m_task))
			return false;

		task.SetPriority(newPriority);
		m_tasks[newPriority].push_back(task);
	}

// ������� ��������� � �������, �� ��� ������� �� ����� ������� ��� ���������� �������
	template <typename T>
	bool DiscardTask(TaskProxy<T>& task){
		if(task.GetState() != TS_QUEUED)	// ������� ������ ��������
			return false;
		task.SetState(TS_DISCARDED);
		return true;
	}


	void		WaitAllTasksCompleted() const;
	bool		AreAllTasksCompleted() const;
	int			GetQueuedTaskCount() const;

	void		SetSleepTime(long milliseconds);
	bool		SetMaxThreadCount(uint8_t maxThreadCount);	// ���� ���������� ������� GetThreadsCount() ������ maxThreadCount, ��������� false 
	uint8_t		GetMaxThreadCount() const;
	void		SetMaxTaskCount(uint8_t maxTaskCount);		// ������������ ���-�� ������ � ������� ������ ������
	uint8_t		GetMaxTaskCount() const;
	float		GetLoadPercent() const;						// ������� ������������� �������

	uint8_t		GetThreadsCount() const;					// ���������� ���������� ������� � ������� (workers + lentThreads + masterThread)
	uint8_t		GetWorkersCount() const;					
	uint8_t		GetLentThreadsCount() const;
	uint8_t		GetCondemnedThreadsCount() const;

	uint8_t		SetWorkersCount(uint8_t _count);			
	uint8_t		IncreaseWorkersCount(uint8_t _count=1);		// ������� count �������������� �������
	uint8_t		DecreaseWorkersCount(uint8_t _count=1);		// ������� count �������
	ThreadPtr	LendThread(bool createNew=false);			// �������� ����� ��� ������ ��������� ����������
	void		ReturnThread(ThreadPtr& th);				// ������� ����� �������

	void		SetMakeBalancing(bool balancing);			// ������ ��� ��� ������������ ��������

private:
	ThreadPtr	FindMostFreeWorker() const;					// ����� �������� � ���������� ���������
	ThreadPtr	FindMostBusyWorker() const;					// ����� �������� � ���������� ���������
	bool		TryGetNextTask(ITaskPtr& task);				// ����� ������� � ����������� ��������� �����������

	void		TaskAssignement();							// ������������� ������� �� �������
	void		ThreadBalancing();							// ������������ �������� �������
	void		JoinCondemnedThreads();
	void		MasterJob();								// ������� ������-������
	void		LaunchMasterThread();

	ThreadPtr	ReleaseWorkerThread();						// ����������� ������ ���������� �������� �� ������
	void		GrabAllTasks(ThreadPtr& th);				// ������� ������� ������ � ��������� � ����� �������

private:
	Thread						m_master;					// ������-�����
	ThreadList					m_workers;					// ������-������� ��� ���� �������
	ThreadList					m_lentThreads;				// ������, ������� �������� ������ �����������. �� ��������� � ����� ������
	ThreadList					m_condemnedThreads;			// ������, ������� ���������� �������
	TaskDeque					m_tasks[TP_COUNT];			// ������� ������� �� �����������

	ITaskPtr					m_masterTask;
	std::chrono::milliseconds	m_sleepTime;	
	std::atomic_bool			m_masterIsFree;	
	std::atomic_bool			m_hasNewTask;				// ���� �� ����� ������� � �������
	std::atomic_bool			m_makeBalancing;			// ������ ��� ��� ������������ ��������	

	uint8_t						m_minWorkersCount;
	uint8_t						m_maxThreadCount;			
	uint8_t						m_maxTaskCount;				// ������������ ���-�� ������� � ������� ������ ������
};


#pragma region TaskProxy
// �� ���� ������� ������ ��������� � ��������� ������� ������
template <typename T>
class TaskProxy{
	friend class ThreadManager;
private:
	TaskProxy(){}

public:
	TaskProxy(std::shared_ptr<Task<T>> task){
		m_task = task;
	}
	TaskProxy(const TaskProxy& taskProxy){
		m_task = taskProxy.m_task;
	}
	TaskProxy(TaskProxy&& taskProxy){
		m_task = std::move(taskProxy.m_task);
	}
	~TaskProxy(){}


	void Perform(){ 
		m_task->Perform(); 
	}

	T GetResult() const{
		return m_task->GetResult();
	}

	void Wait() const{
		m_task->Wait();
	}

	TaskState GetState() const{
		return m_task->GetState();
	}

	TaskPriority GetPriority() const{
		return m_task->GetPriority();
	}

private:
	void SetState(TaskState state){
		m_task->SetState(state);
	}

	void SetPriority(TaskPriority priority){
		m_task->SetPriority(priority);
	}

private:
	std::shared_ptr<Task<T>> m_task;
};
#pragma endregion

#pragma region MultitaskProxy
template <typename T>
class MultitaskProxy{
public:
	MultitaskProxy(std::shared_ptr<Multitask<T>> multitask){
		m_multitask = multitask;
	}

	void Wait() const{
		m_multitask->Wait();
	}

	TaskState GetTaskState() const{
		return m_multitask->GetTaskState();
	}

	int GetTaskCount() const{
		return m_multitask->GetTaskCount();
	}

	TaskProxy<T> GetSubtask(int indx){
		return TaskProxy<T>(m_multitask->GetSubtask(indx));
	}

private:
	std::shared_ptr<Multitask<T>> m_multitask;
};
#pragma endregion

}
}