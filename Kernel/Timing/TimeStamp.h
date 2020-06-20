/****************************************************************
*	Нормализованное представление некоторого момента времени	*
****************************************************************/
#pragma once

#include "Timing.h"
#include <string>
#include <chrono>


namespace RGE
{
namespace Timing
{

class KERNEL_API TimeStamp{
public:
	TimeStamp();
	explicit TimeStamp(uint64_t ms);
	TimeStamp(uint32_t h, uint32_t m, uint32_t s, uint32_t ms);
	TimeStamp(const TimeStamp& ts);
	template <class Clock> 
	explicit TimeStamp(const std::chrono::time_point<Clock>& timePoint){
		this->Set<Clock>(timePoint);
	}
	
	void		Set(uint64_t ms);
	void		Set(uint32_t h, uint32_t m, uint32_t s, uint32_t ms);
	template <class Clock> 
	void Set(const std::chrono::time_point<Clock>& timePoint){
		uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>( timePoint.time_since_epoch() ).count();
		this->Set(ms);
	}

	void		Normalize();							// нормализовать время (h: [0-23], m: [0-59], s: [0-59], ms: [0-999])
	uint64_t	GetDuration() const;					// перевод нормализованного времени в миллисекунды
	std::string ToString() const;						// строковое представление вида "hh:mm:ss.ms"

	TimeStamp&	operator=(const TimeStamp& rhs);
	TimeStamp&	operator+=(const TimeStamp& rhs);
	TimeStamp&	operator-=(const TimeStamp& rhs);		// NOTE: если rhs > this, то result = 0

	TimeStamp	operator+(const TimeStamp& rhs) const;
	TimeStamp	operator-(const TimeStamp& rhs) const;	// NOTE: если rhs > this, то result = 0

	bool		operator<(const TimeStamp& rhs) const;
	bool		operator>(const TimeStamp& rhs) const;
	bool		operator<=(const TimeStamp& rhs) const;
	bool		operator>=(const TimeStamp& rhs) const;
	bool		operator==(const TimeStamp& rhs) const;
	bool		operator!=(const TimeStamp& rhs) const;

	operator std::string() const;						

public:
	uint32_t	hours;
	uint32_t	minutes;
	uint32_t	seconds;
	uint32_t	milliseconds;
};

}
}