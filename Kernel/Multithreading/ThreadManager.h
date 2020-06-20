/************************************************************
*				Класс для работы с потоками;				*
*	содежит пул потоков и распределяет нагрузку между ними;	*
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
		auto newTask = std::make_shared<Task<T>>(f);		//TODO: создавать таски через пул. Где он должен быть определен?

		newTask->SetPriority(priority);
		m_tasks[priority].push_back(newTask);
		LaunchMasterThread();
		
		return TaskProxy<T>(newTask);
	}

	template <typename T>
	MultitaskProxy<T> Execute(std::function<T(int rank, int size)> f, uint8_t numThreads){
		auto newTask = std::make_shared<Multitask<T>>(f, numThreads);		//TODO: создавать таски через пул. Где он должен быть определен?
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
		if(task.GetState() <= TS_PROCESSING)	// задание еще не выполнилось
			return false;

		task.SetState(TS_QUEUED);
		m_tasks[task.GetPriority()].push_back(task.m_task);
		LaunchMasterThread();

		return true;
	}

	template <typename T>
	bool ChangeTaskPriority(TaskProxy<T>& task, TaskPriority newPriority){
		if(task.GetPriority() == newPriority)	return true;
		if(task.GetState() != TS_QUEUED)		return false;	// задание в сотоянии выполнения или отменено

	// скорее всего задание поступило на обработку потоком, но еще находится в его очереди
		if(!m_tasks[task.GetPriority()].try_erase(task.m_task))
			return false;

		task.SetPriority(newPriority);
		m_tasks[newPriority].push_back(task);
	}

// задание останется в очереди, но его функция не будет вызвана при выполнении потоком
	template <typename T>
	bool DiscardTask(TaskProxy<T>& task){
		if(task.GetState() != TS_QUEUED)	// задание поздно отменять
			return false;
		task.SetState(TS_DISCARDED);
		return true;
	}


	void		WaitAllTasksCompleted() const;
	bool		AreAllTasksCompleted() const;
	int			GetQueuedTaskCount() const;

	void		SetSleepTime(long milliseconds);
	bool		SetMaxThreadCount(uint8_t maxThreadCount);	// если невозможно сделать GetThreadsCount() меньше maxThreadCount, возвратит false 
	uint8_t		GetMaxThreadCount() const;
	void		SetMaxTaskCount(uint8_t maxTaskCount);		// максимальное кол-во работы в очереди одного потока
	uint8_t		GetMaxTaskCount() const;
	float		GetLoadPercent() const;						// процент загруженности потоков

	uint8_t		GetThreadsCount() const;					// количество работающих потоков в системе (workers + lentThreads + masterThread)
	uint8_t		GetWorkersCount() const;					
	uint8_t		GetLentThreadsCount() const;
	uint8_t		GetCondemnedThreadsCount() const;

	uint8_t		SetWorkersCount(uint8_t _count);			
	uint8_t		IncreaseWorkersCount(uint8_t _count=1);		// создает count дополнительных потоков
	uint8_t		DecreaseWorkersCount(uint8_t _count=1);		// убирает count потоков
	ThreadPtr	LendThread(bool createNew=false);			// одолжить поток для работы некоторой подсистемы
	void		ReturnThread(ThreadPtr& th);				// вернуть поток обратно

	void		SetMakeBalancing(bool balancing);			// делать или нет балансировку нагрузки

private:
	ThreadPtr	FindMostFreeWorker() const;					// поиск рабочего с наименьшей загрузкой
	ThreadPtr	FindMostBusyWorker() const;					// поиск рабочего с наибольшей загрузкой
	bool		TryGetNextTask(ITaskPtr& task);				// взять задание с максимально возможным приоритетом

	void		TaskAssignement();							// распределение заданий по потокам
	void		ThreadBalancing();							// балансировка загрузки потоков
	void		JoinCondemnedThreads();
	void		MasterJob();								// функция мастер-потока
	void		LaunchMasterThread();

	ThreadPtr	ReleaseWorkerThread();						// освобождает самого свободного рабочего от работы
	void		GrabAllTasks(ThreadPtr& th);				// забрать задания потока и поместить в общую очередь

private:
	Thread						m_master;					// мастер-поток
	ThreadList					m_workers;					// потоки-рабочие для всей системы
	ThreadList					m_lentThreads;				// потоки, которых одолжили другим подсистемам. Не участвуют в общей работе
	ThreadList					m_condemnedThreads;			// потоки, которые необходимо удалить
	TaskDeque					m_tasks[TP_COUNT];			// очереди заданий по приоритетам

	ITaskPtr					m_masterTask;
	std::chrono::milliseconds	m_sleepTime;	
	std::atomic_bool			m_masterIsFree;	
	std::atomic_bool			m_hasNewTask;				// есть ли новые задания в очереди
	std::atomic_bool			m_makeBalancing;			// делать или нет балансировку нагрузки	

	uint8_t						m_minWorkersCount;
	uint8_t						m_maxThreadCount;			
	uint8_t						m_maxTaskCount;				// максимальное кол-во заданий в очереди одного потока
};


#pragma region TaskProxy
// не дает клиенту менять приоритет и состояние задания самому
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