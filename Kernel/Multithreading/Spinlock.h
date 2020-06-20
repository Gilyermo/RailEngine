/********************************************************
*	спин-блокировка с возможностью усыпления потока		*
********************************************************/
#pragma once

#include "Multithreading.h"
#include <atomic>
#include <thread>


namespace RGE
{
namespace Multithreading
{

class Spinlock{
public:
	Spinlock(uint32_t sleepTime_ms = 0, uint32_t busyWaitTriesCount = 50){
		m_locked.clear();
		m_sleepTime			=	std::chrono::milliseconds(sleepTime_ms);
		m_busyWaitTriesCount=	busyWaitTriesCount;
	}
	~Spinlock(){
		m_locked.clear();
	}

	void lock(){
		uint32_t tryCount = m_busyWaitTriesCount;
		while(tryCount--) {
			if(!m_locked.test_and_set())
				return;
		}

		while(true){
			if(!m_locked.test_and_set())
				return;
			std::this_thread::sleep_for(m_sleepTime);
		}
	}

	bool try_lock(){
		return m_locked.test_and_set();
	}

	void unlock(){
		m_locked.clear();
	}


	void set_sleep_time(uint32_t sleepTime_ms){
		m_sleepTime = std::chrono::milliseconds(sleepTime_ms);
	}

	void set_busy_wait_tries_count(uint32_t busyWaitTriesCount) {
		m_busyWaitTriesCount = busyWaitTriesCount;
	}

private:
	std::atomic_flag			m_locked;
	std::chrono::milliseconds	m_sleepTime;
	uint32_t					m_busyWaitTriesCount;
};

}
}