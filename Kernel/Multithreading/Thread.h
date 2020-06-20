/********************************************
*	поток с собственной очередью заданий	*
********************************************/
#pragma once

#include "Multithreading.h"
#include "SafeContainer.h"
#include "Task.h"
#include <functional>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>


namespace RGE
{
namespace Multithreading
{

class KERNEL_API Thread{
private:
	typedef SafeContainer< std::deque<ITaskPtr> >	TaskDeque;

private:
	Thread(const Thread&) = delete;
	Thread(Thread&&) = delete;

	static std::atomic<uint16_t>	m_totalThreadCount;

public:
	Thread();
	~Thread();

	void							Perform(const ITaskPtr& task);		// ставит задание в очередь на выполнение
	std::shared_ptr<Task<void>>		Execute(std::function<void()> f);
	bool							TryGetLastTask(ITaskPtr& task);		// забирает последнее задание из очереди
	size_t							GetTaskCount() const;			

	void							Join();
	void							Detach();
	void							Suspend();							// поток засыпает после завершения выполняемой в данный момент функции
	void							Resume();

	bool							IsFree() const;
	bool							IsActive() const;
	bool							IsAlive() const;

	uint16_t						LocalID() const;

private:
	void							ThreadFunction();

private:
	TaskDeque						m_tasks;
	std::thread						m_thread;

	std::mutex						m_mutex;
	std::condition_variable			m_cond;

	std::atomic_bool				m_enable;
	std::atomic_bool				m_active;
	std::atomic_bool				m_free;	

	uint16_t						m_localID;
};

typedef std::shared_ptr<Thread>		ThreadPtr;

}
}