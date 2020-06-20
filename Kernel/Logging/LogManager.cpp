#include "LogManager.h"
#include "LogFile.h"
#include "..\Misc\FormatString.h"
#include "..\Memory\PoolAllocator.h"
#include "..\Memory\Adapter.h"


namespace RGE
{
namespace Logging
{

Memory::PoolAllocator<LogFile>		g_logFilePool(5);


LogManager::LogManager()
	: m_logsFolder("Logs\\"), 
	m_messageTypeEnabled(MT_Info | MT_Warning | MT_Error | MT_Debug)
{}

LogManager::~LogManager(){
	for(auto& curLog : m_logs)
		curLog.second.reset();
}


LogManager& LogManager::Instance(){
	static LogManager singleton;
	return singleton;
}


LogHandle LogManager::CreateLog(const std::string& name, LogType logType){
	LogHandle handle = -1;
	std::string filepath = m_logsFolder + name + ".log";

	Multithreading::WriteLockGuard lock(m_rwLock);
	switch(logType){
	case LT_File:
		m_logs.emplace_back( std::make_pair(name, Memory::CreateShared<LogFile>(g_logFilePool, filepath)) );
		handle = LogHandle(m_logs.size() - 1);
		break;
	}

	return handle;
}

void LogManager::SetLogsFolder(const std::string& path){
	Multithreading::WriteLockGuard lock(m_rwLock);
	m_logsFolder = path;
}

const std::string& LogManager::GetLogsFolder() const{
	return m_logsFolder;
}


void LogManager::LogMessage(uint32_t messageType, LogHandle log, const char* fmt, ...){
	RGE_Assert(log>=0 && log<LogHandle(m_logs.size()), Exception::WrongArgument, "There's no log with such handle");

	if(IsMessageTypeEnabled(messageType)){
//TODO: воткнуть вывод типа сообщения в лог
		std::string msg;

		va_list args;
		va_start(args, fmt);
		Misc::vFormatString(msg, fmt, args);
		va_end(args);

		*m_logs[log].second << msg;
	}
}

void LogManager::LogMessage(uint32_t messageType, LogHandle log, std::string& msg){
	RGE_Assert(log>=0 && log<LogHandle(m_logs.size()), Exception::WrongArgument, "There's no log with such handle");

	if(IsMessageTypeEnabled(messageType)){
//TODO: воткнуть вывод типа сообщения в лог
		*m_logs[log].second << msg;
	}
}

void LogManager::LogMessage(uint32_t messageType, LogHandle log, Exception::BaseException& e){
	RGE_Assert(log>=0 && log<LogHandle(m_logs.size()), Exception::WrongArgument, "There's no log with such handle");

	if(IsMessageTypeEnabled(messageType)){
//TODO: воткнуть вывод типа сообщения в лог
		*m_logs[log].second << e;
	}
}


void LogManager::LogInfo(LogHandle log, const char* fmt, ...){
	LogMessage(MT_Info, log, fmt);
}

void LogManager::LogInfo(LogHandle log, std::string& msg){
	LogMessage(MT_Info, log, msg);
}

void LogManager::LogInfo(LogHandle log, Exception::BaseException& e){
	LogMessage(MT_Info, log, e);
}


void LogManager::LogWarning(LogHandle log, const char* fmt, ...){
	LogMessage(MT_Warning, log, fmt);
}

void LogManager::LogWarning(LogHandle log, std::string& msg){
	LogMessage(MT_Warning, log, msg);
}

void LogManager::LogWarning(LogHandle log, Exception::BaseException& e){
	LogMessage(MT_Warning, log, e);
}


void LogManager::LogError(LogHandle log, const char* fmt, ...){
	LogMessage(MT_Error, log, fmt);
}

void LogManager::LogError(LogHandle log, std::string& msg){
	LogMessage(MT_Error, log, msg);
}

void LogManager::LogError(LogHandle log, Exception::BaseException& e){
	LogMessage(MT_Error, log, e);
}


void LogManager::LogDebug(LogHandle log, const char* fmt, ...){
	LogMessage(MT_Debug, log, fmt);
}

void LogManager::LogDebug(LogHandle log, std::string& msg){
	LogMessage(MT_Debug, log, msg);
}

void LogManager::LogDebug(LogHandle log, Exception::BaseException& e){
	LogMessage(MT_Debug, log, e);
}


ILog& LogManager::Log(LogHandle log){
	RGE_Assert(log>=0 && log<LogHandle(m_logs.size()), Exception::WrongArgument, "There's no log with such handle");
	return *m_logs[log].second;
}
	
const ILog& LogManager::Log(LogHandle log) const{
	RGE_Assert(log>=0 && log<LogHandle(m_logs.size()), Exception::WrongArgument, "There's no log with such handle");
	return *m_logs[log].second;
}

ILog& LogManager::Log(const std::string& name){
	return Log( GetHandleByName(name) );
}

const ILog& LogManager::Log(const std::string& name) const{
	return Log( GetHandleByName(name) );
}

LogHandle LogManager::GetHandleByName(const std::string& name) const{
	Multithreading::ReadLockGuard lock(m_rwLock);
	for(size_t i=0; i<m_logs.size(); i++)
		if(m_logs[i].first == name)
			return LogHandle(i);
	return LogHandle(-1);
}

}
}