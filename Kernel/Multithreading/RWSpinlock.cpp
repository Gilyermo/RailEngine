#include "RWSpinlock.h"


namespace RGE
{
namespace Multithreading
{

RWSpinlock::RWSpinlock(uint32_t sleepTime_ms){
	m_sleepTime = std::chrono::milliseconds(sleepTime_ms);
	m_lockValue.store(0);
}

RWSpinlock::~RWSpinlock(){
	m_lockValue.store(0);
}


void RWSpinlock::LockRead(){
	while(true){
		while(m_lockValue & 0xFFF00000)								// пока есть писатели - ждем
			std::this_thread::sleep_for(m_sleepTime);
			
		if( !(++m_lockValue & 0xFFF00000) )							// если не произошло захвата на запись - все хорошо
			return;
		m_lockValue--;												// иначе уменьшаем число чтецов и пробуем снова
	}
}

bool RWSpinlock::TryLockRead(){
	if( !(++m_lockValue & 0xFFF00000) )
		return true;

	m_lockValue--;
	return false;
}

void RWSpinlock::UnlockRead(){
	RGE_Assert(m_lockValue & 0x000FFFFF, Exception::WrongState, "There are no read locks");
	m_lockValue--;
}

bool RWSpinlock::IsLockedRead() const{
	return (m_lockValue & 0x000FFFFF) != 0;
}


void RWSpinlock::LockWrite(){
	while(true){
		while(m_lockValue & 0xFFF00000)								// пока есть другие писатели - ждем
			std::this_thread::sleep_for(m_sleepTime);

		m_lockValue += 0x00100000;
		if( (m_lockValue & 0xFFF00000) == 0x00100000 ){
			while(m_lockValue & 0x000FFFFF)							// ждем, пока есть читающие потоки
				std::this_thread::sleep_for(m_sleepTime);
			return;
		}
		m_lockValue -= 0x00100000;
	}
}

bool RWSpinlock::TryLockWrite(){
	uint32_t oldVal = 0;
	uint32_t newVal = 0x00100000;
	return m_lockValue.compare_exchange_strong(oldVal, newVal);
}

void RWSpinlock::UnlockWrite(){
	RGE_Assert(m_lockValue & 0xFFF00000, Exception::WrongState, "There are no write locks");
	m_lockValue -= 0x00100000;
}

bool RWSpinlock::IsLockedWrite() const{
	return (m_lockValue & 0xFFF00000) != 0;
}

void RWSpinlock::SetSleepTime(uint32_t sleepTime_ms){
	m_sleepTime = std::chrono::milliseconds(sleepTime_ms);
}

}
}