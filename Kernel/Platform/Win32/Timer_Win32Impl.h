/************************************************
*	������ �� ������ QueryPerformanceCounter	*
************************************************/
#pragma once

#include "..\Settings.h"
#ifdef RGE_WINDOWS

#include "../../Timing/ITimer.h"
#include <Windows.h>


namespace RGE
{
namespace Timing
{

class KERNEL_API Win32Timer : public ITimer{
public:
	Win32Timer(bool paused = false);
	~Win32Timer();

	void		Start();		// ��������� ������
	void		Stop();			// �������� ������ � ��������� ������ �� �����
	void		Pause();		// ��������� ������ �� �����. ����� �� ������������
	void		Reset();		// �������� ������� ������� 
	
// �������� ����� � ������� ������ � ���������� ���������� ����������� �� ������� �������
	uint32_t	MeasureTime();
	uint32_t	GetElapsed() const;

	TimeStamp	GetTimeStamp() const;

private:
	LARGE_INTEGER	m_frequency;
	LARGE_INTEGER	m_lastTimePoint;
	uint32_t		m_elapsed;
	bool			m_isPaused;
};

}
}


#endif		// RGE_WINDOWS