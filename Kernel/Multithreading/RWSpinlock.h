/****************************************
*	спин-блокировка на чтение-запись	*
****************************************/
#pragma once

#include "Multithreading.h"
#include "../Exception/Exception.h"
#include <atomic>
#include <thread>


namespace RGE
{
namespace Multithreading
{

class KERNEL_API IReadWriteLock{
public:
	virtual ~IReadWriteLock(){}

	virtual void LockRead() = 0;
	virtual void UnlockRead() = 0;
	virtual void LockWrite() = 0;
	virtual void UnlockWrite() = 0;
};


class KERNEL_API RWSpinlock : public IReadWriteLock{
private:
	RWSpinlock(const RWSpinlock&) = delete;
	RWSpinlock& operator=(const RWSpinlock&) = delete;

public:
	RWSpinlock(uint32_t sleepTime_ms = 0);
	~RWSpinlock();

	void LockRead();
	bool TryLockRead();
	void UnlockRead();
	bool IsLockedRead() const;

	void LockWrite();
	bool TryLockWrite();
	void UnlockWrite();
	bool IsLockedWrite() const;

	void SetSleepTime(uint32_t sleepTime_ms);

private:
	std::atomic<uint32_t>		m_lockValue;		// 12 старших бит отведено для пишущих потоков, остальные - читающим
	std::chrono::milliseconds	m_sleepTime;
};


//==================================
//		Read/Write Lock Guard
//==================================
class ReadLockGuard{
private:
	ReadLockGuard(const ReadLockGuard&) = delete;					
	ReadLockGuard& operator=(const ReadLockGuard&) = delete;

public:
	ReadLockGuard(IReadWriteLock& rwlock) : m_rwlock(rwlock){
		m_rwlock.LockRead();
	}

	~ReadLockGuard(){
		m_rwlock.UnlockRead();
	}

private:
	IReadWriteLock&		m_rwlock;
};


class WriteLockGuard{
private:
	WriteLockGuard(const WriteLockGuard&) = delete;
	WriteLockGuard& operator=(const WriteLockGuard&) = delete;

public:
	WriteLockGuard(IReadWriteLock& rwlock) : m_rwlock(rwlock){
		m_rwlock.LockWrite();
	}

	~WriteLockGuard(){
		m_rwlock.UnlockWrite();
	}

private:
	IReadWriteLock&		m_rwlock;
};
//==================================


}
}