/************************************************
*	класс для работы с заданиями;				*
*	позволяет отслеживать состояние задачи,		*
*	получать результат выполнения функции,		*
*	обеспечивает передачу исключения наружу;	*
*	функция должна быть вида: T f();			*
************************************************/
#pragma once

#include "Multithreading.h"
#include "ITask.h"
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <exception>


namespace RGE
{
namespace Multithreading
{

template <typename T>
class Task : public ITask{
private:
	Task() = delete;
	Task(const Task& t) = delete;
	Task(Task&& t) = delete;

	Task& operator=(const Task&) = delete;

public:
	Task(std::function<T()> f){
		m_f = f;
		m_state.store(TS_QUEUED);
	}
	~Task(){}

// исполняет функцию в вызвавшем потоке
	void Perform(){
		TaskState expected = TS_QUEUED;
		if(!m_state.compare_exchange_strong(expected, TS_PROCESSING))
			return;

		try{
			m_result = m_f();
		}
		catch(...){
			m_exception = std::current_exception();
		}
		
		std::lock_guard<std::mutex> lock(m_mutex);
		m_state.store(TS_READY);
		m_cond.notify_all();
	}	

// возвращает результат функции; если он еще не готов - ждет
	T GetResult() const{
		Wait();

		if(m_exception)
			std::rethrow_exception(m_exception);
		return m_result;
	}	

// вызвавший поток ожидает завершения задания
	void Wait() const{
		if(m_state >= TS_READY)	return;

		std::unique_lock<std::mutex> lock(m_mutex);
		m_cond.wait( lock, [&](){return m_state >= TS_READY;} );
	}

	TaskState GetState() const{
		return m_state;
	}
	
	void SetState(TaskState state){
		m_state = state;
	}

	TaskPriority GetPriority() const{
		return m_priority;
	}

	void SetPriority(TaskPriority priority){
		m_priority.store(priority);
	}

private:
	std::function<T()>				m_f;	

	std::atomic<TaskState>			m_state;
	std::atomic<TaskPriority>		m_priority;
	mutable std::mutex				m_mutex;
	mutable std::condition_variable	m_cond;

	std::exception_ptr				m_exception;
	T								m_result;
};


//======================================
// Специализация класса для типа T&
//======================================
template<typename T>
class Task<T&> : public ITask{
private:
	Task(const Task& t){}
	Task(Task&& t){}

public:
	Task(std::function<T&()> f){
		m_f = f;
		m_state.store(TS_QUEUED);
	}
	~Task(){}


	void Perform(){
		TaskState expected = TS_QUEUED;
		if(!m_state.compare_exchange_strong(expected, TS_PROCESSING))
			return;

		try{
			m_result = &m_f();
		}
		catch(...){
			m_exception = std::current_exception();
		}

		m_state = TS_READY;
		m_cond.notify_one();
	}	

	T& GetResult() const{
		Wait();

		if(m_exception)
			std::rethrow_exception(m_exception);
		return *m_result;
	}	

	void Wait() const{
		if(m_state >= TS_READY)	return;

		std::unique_lock<std::mutex> lock(m_mutex);
		m_cond.wait( lock, [&](){return m_state >= TS_READY;} );
	}

	TaskState GetState() const{
		return m_state;
	}
	
	void SetState(TaskState state){
		m_state = state;
	}

	TaskPriority GetPriority() const{
		return m_priority;
	}

	void SetPriority(TaskPriority priority){
		m_priority.store(priority);
	}

private:
	std::function<T&()>				m_f;	

	std::atomic<TaskState>			m_state;
	std::atomic<TaskPriority>		m_priority;
	mutable std::mutex				m_mutex;
	mutable std::condition_variable	m_cond;

	std::exception_ptr				m_exception;
	T*								m_result;
};


//======================================
// Специализация класса для типа void
//======================================
template<>
class Task<void> : public ITask{
private:
	Task(const Task&){}
	Task(Task&&){}

public:
	Task(std::function<void()> f){
		m_f = f;
		m_state = TS_QUEUED;
	}
	~Task(){}


	void Perform(){
		TaskState expected = TS_QUEUED;
		if(!m_state.compare_exchange_strong(expected, TS_PROCESSING))
			return;

		try{
			m_f();
		}
		catch(...){
			m_exception = std::current_exception();
		}

		std::unique_lock<std::mutex> lock(m_mutex);
		m_state = TS_READY;
		m_cond.notify_one();
	}	

	void GetResult() const{
		Wait();

		if(m_exception)
			std::rethrow_exception(m_exception);
	}	

	void Wait() const{
		if(m_state >= TS_READY)	return;

		std::unique_lock<std::mutex> lock(m_mutex);
		m_cond.wait( lock, [&](){return m_state >= TS_READY;} );
	}

	TaskState GetState() const{
		return m_state;
	}
	
	void SetState(TaskState state){
		m_state = state;
	}

	TaskPriority GetPriority() const{
		return m_priority;
	}

	void SetPriority(TaskPriority priority){
		m_priority.store(priority);
	}

private:
	std::function<void()>			m_f;	

	std::atomic<TaskState>			m_state;
	std::atomic<TaskPriority>		m_priority;
	mutable std::mutex				m_mutex;
	mutable std::condition_variable	m_cond;

	std::exception_ptr				m_exception;
};

}
}