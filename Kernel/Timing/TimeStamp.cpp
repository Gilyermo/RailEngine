#include "TimeStamp.h"
#include "..\Misc\FormatString.h"


namespace RGE
{
namespace Timing
{

TimeStamp::TimeStamp() 
	: hours(0),
	minutes(0),
	seconds(0),
	milliseconds(0)
{}

TimeStamp::TimeStamp(uint64_t ms){
	Set(ms);
}

TimeStamp::TimeStamp(uint32_t h, uint32_t m, uint32_t s, uint32_t ms)
	: hours(h),
	minutes(m),
	seconds(s),
	milliseconds(ms)
{
	Normalize();
}

TimeStamp::TimeStamp(const TimeStamp& ts)
	: hours(ts.hours),
	minutes(ts.minutes),
	seconds(ts.seconds),
	milliseconds(ts.milliseconds)
{}


void TimeStamp::Set(uint64_t ms){
	hours			=	static_cast<uint32_t>( ms / (1000 * 60 * 60) );
	minutes			=	static_cast<uint32_t>( ms / (1000 * 60) - hours * 60 );
	seconds			=	static_cast<uint32_t>( ms / 1000 - (minutes + hours * 60) * 60 );
	milliseconds	=	static_cast<uint32_t>( ms - ( seconds + (minutes  + hours*60) * 60 ) * 1000 );
	hours			%=	24;
}

void TimeStamp::Set(uint32_t h, uint32_t m, uint32_t s, uint32_t ms){
	hours			=	h;
	minutes			=	m;
	seconds			=	s;
	milliseconds	=	ms;
	Normalize();
}


void TimeStamp::Normalize(){
	uint32_t extraTime;

	extraTime		=	milliseconds / 1000;
	milliseconds	-=	extraTime * 1000;
	seconds			+=	extraTime;

	extraTime		=	seconds / 60;
	seconds			-=	extraTime * 60;
	minutes			+=	extraTime;

	extraTime		=	minutes / 60;
	minutes			-=	extraTime * 60;
	hours			+=	extraTime;
	hours			%=	24;
}

uint64_t TimeStamp::GetDuration() const{
	return milliseconds + ( seconds + (minutes + hours * 60) * 60 ) * 1000;
}

std::string TimeStamp::ToString() const{
	char buf[20];		// всего нужно 13 байт, но берем с запасом
	Misc::FormatString(buf, 20, "%2i:%2i:%2i.%3i", hours, minutes, seconds, milliseconds);
	return std::string(buf);
}


TimeStamp& TimeStamp::operator=(const TimeStamp& rhs){
	if(this != &rhs){
		hours		=	rhs.hours;
		minutes		=	rhs.minutes;
		seconds		=	rhs.seconds;
		milliseconds=	rhs.milliseconds;
	}
	return *this;
}

TimeStamp& TimeStamp::operator+=(const TimeStamp& rhs){
	this->Set(this->GetDuration() + rhs.GetDuration());
	return *this;
}

TimeStamp& TimeStamp::operator-=(const TimeStamp& rhs){
	uint64_t this_ms	=	this->GetDuration();
	uint64_t rhs_ms		=	rhs.GetDuration();
	
	if(this_ms <= rhs_ms)	this->Set(0, 0, 0, 0);
	else 					this->Set(this_ms - rhs_ms);
	
	return *this;
}


TimeStamp TimeStamp::operator+(const TimeStamp& rhs) const{
	TimeStamp res(this->GetDuration() + rhs.GetDuration());
	return res;
}

TimeStamp TimeStamp::operator-(const TimeStamp& rhs) const{
	TimeStamp res;
	uint64_t this_ms	=	this->GetDuration();
	uint64_t rhs_ms		=	rhs.GetDuration();
	
	if(this_ms > rhs_ms)	res.Set(this_ms - rhs_ms);
	return res;
}


bool TimeStamp::operator<(const TimeStamp& rhs) const{
	return this->GetDuration() < rhs.GetDuration();
}

bool TimeStamp::operator>(const TimeStamp& rhs) const{
	return this->GetDuration() > rhs.GetDuration();
}

bool TimeStamp::operator<=(const TimeStamp& rhs) const{
	return this->GetDuration() <= rhs.GetDuration();
}

bool TimeStamp::operator>=(const TimeStamp& rhs) const{
	return this->GetDuration() >= rhs.GetDuration();
}

bool TimeStamp::operator==(const TimeStamp& rhs) const{
	return this->GetDuration() == rhs.GetDuration();
}

bool TimeStamp::operator!=(const TimeStamp& rhs) const{
	return this->GetDuration() != rhs.GetDuration();
}


TimeStamp::operator std::string() const{
	return this->ToString();
}

}
}
