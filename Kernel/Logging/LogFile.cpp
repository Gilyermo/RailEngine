#include "LogFile.h"
#include "..\Multithreading\ThreadManager.h"
#include "..\Timing\TimeStamp.h"
#include "..\Misc\FormatString.h"
#include <stdarg.h>
#include <chrono>


namespace RGE
{
namespace Logging
{

using namespace Multithreading;


LogFile::LogFile(const std::string& filename, uint32_t bufferSize, uint32_t spinSleep_ms)
	: m_filename(filename),
	m_writeTime(true),
	m_writeThreadID(true),
	m_lock(spinSleep_ms)
{
	m_file.open(m_filename);

	RGE_Assert(m_file, Exception::WrongState, ("Couldn't open file " + filename));
}

LogFile::~LogFile(){
	Flush();
	m_file.close();
}


void LogFile::SetWriteTime(bool b){
	m_writeTime = b;
}

void LogFile::SetWriteThreadID(bool b){
	m_writeThreadID = b;
}


void LogFile::Write(const char* fmt, ...){
	std::lock_guard<Multithreading::Spinlock> lock(m_lock);
	WritePrefix();
	
	std::string buf;
	va_list args;

	va_start(args, fmt);
	Misc::vFormatString(buf, fmt, args);
	va_end(args);

	m_file << buf <<"\n";
}

void LogFile::operator<<(const char* str){
	std::lock_guard<Multithreading::Spinlock> lock(m_lock);
	WritePrefix();
	
	m_file << str << "\n";
}

void LogFile::operator<<(const std::string& str){
	std::lock_guard<Multithreading::Spinlock> lock(m_lock);
	WritePrefix();
	
	m_file << str << "\n";
}

void LogFile::operator<<(const Exception::BaseException& e){
	std::lock_guard<Multithreading::Spinlock> lock(m_lock);
	WritePrefix();
	
	m_file << e.What() << "\n";
}


void LogFile::Flush(){
	m_file.flush();
}


const std::string& LogFile::GetFilename() const{
	return m_filename;
}


void LogFile::WritePrefix(){
	if(m_writeTime || m_writeThreadID){
		if(m_writeTime){
			Timing::TimeStamp now( std::chrono::system_clock::now() );
			m_file << now.ToString();
		}

		if(m_writeThreadID){
			if(m_writeTime)		
				m_file << " ";
			m_file << "[" << std::this_thread::get_id() << "]";
		}

		m_file << " - ";
	}
}

}
}