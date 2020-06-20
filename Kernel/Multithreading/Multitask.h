/************************************************************************************************
*	Класс для работы с заданиями, которые должны выполняться в нескольких потоках одновременно;	*
*	такое задание разбивается на несколько подзаданий, которые скармливаются доступным потокам,	*
*	причем, количество потоков может быть меньше числа подзаданий;								*
*	обеспечивает отслеживание состояния задачи и получение результатов от выполнившихся подзадач*
************************************************************************************************/
#pragma once

#include "Multithreading.h"
#include "Task.h"
#include <vector>
#include <memory>


namespace RGE
{
namespace Multithreading
{

template <typename T>
class Multitask{
private:
	Multitask() = delete;

public:
	Multitask(std::function<T(int, int)> f, int n){
		m_taskCount = n;
		m_tasks.resize(m_taskCount);
		for(int i=0; i<m_taskCount; i++)
			m_tasks[i] = std::make_shared<Task<T>>( std::bind(f, i, m_taskCount) );			//TODO: создавать таски через распределители памяти
	}

	Multitask(const Multitask& multitask){
		m_tasks = multitask.m_tasks;
	}

	Multitask(Multitask&& multitask){
		m_tasks = std::move( multitask.m_tasks );
	}
	
	~Multitask(){}


	void Wait() const{
		auto itEnd = m_tasks.end();
		for(auto it=m_tasks.begin(); it!=itEnd; ++it)
			(*it)->Wait();
	}

	TaskState GetTaskState() const{
		TaskState state = TS_COUNT;
		auto itEnd = m_tasks.end();
		for(auto it=m_tasks.begin(); it!=itEnd && state>TS_QUEUED; ++it)
			state = std::min(state, (*it)->GetState());
		return state;
	}

	int GetTaskCount() const{
		return m_taskCount;
	}


	std::shared_ptr<Task<T>> GetSubtask(int indx){
		return m_tasks[indx];
	}

	Multitask& operator=(const Multitask& multitask){
		if(this != multitask){
			m_tasks = multitask.m_tasks;
		}
		return *this;
	}

private:
	std::vector< std::shared_ptr<Task<T>> >		m_tasks;
	int											m_taskCount;
};

}
}