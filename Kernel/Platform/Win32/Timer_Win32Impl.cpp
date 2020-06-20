#include "Timer_Win32Impl.h"
#include "..\..\Timing\TimeStamp.h"


namespace RGE
{
namespace Timing
{

Win32Timer::Win32Timer(bool paused) : 
	m_isPaused(paused), 
	m_elapsed(0)
{
	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_lastTimePoint);
}

Win32Timer::~Win32Timer(){}


void Win32Timer::Start(){
	if(!m_isPaused)	return;

	QueryPerformanceCounter(&m_lastTimePoint);
	m_isPaused = false;
}

void Win32Timer::Stop(){
	m_elapsed	=	0;
	m_isPaused	=	true;
}

void Win32Timer::Pause(){
	if(m_isPaused)	return;

	MeasureTime();
	m_isPaused = true;
}

void Win32Timer::Reset(){
	m_elapsed = 0;
	QueryPerformanceCounter(&m_lastTimePoint);
}


uint32_t Win32Timer::MeasureTime(){
	LARGE_INTEGER curTime;
	QueryPerformanceCounter(&curTime);

	m_elapsed		+=	static_cast<uint32_t>( (curTime.QuadPart - m_lastTimePoint.QuadPart) / m_frequency.QuadPart );
	m_lastTimePoint =	curTime;

	return GetElapsed();
}

uint32_t Win32Timer::GetElapsed() const{
	return m_elapsed;
}

TimeStamp Win32Timer::GetTimeStamp() const{
	return TimeStamp( GetElapsed() );
}

}
}