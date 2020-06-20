/************************************************************************************
*			Предоставляет доступ ко всем зарегистрированным логам в системе;		*
*	управляет выводом сообщений в зависимости от установленного уровня подробности;	*
*	позволяет назначать несколько типов логов (файл, консоль,...) на один хэндл		*
*	(в этом случае сообщения будут дублироваться в каждый из назначенных логов)		*
************************************************************************************/
#pragma once

#include "Logging.h"
#include "..\Exception\Exception.h"
#include "..\Multithreading\RWSpinlock.h"
#include <cstdint>
#include <string>
#include <vector>


namespace RGE
{
namespace Logging
{

#define RGE_LogInfo			RGE::Logging::LogManager::Instance().LogInfo
#define RGE_LogWarning		RGE::Logging::LogManager::Instance().LogWarning
#define RGE_LogError		RGE::Logging::LogManager::Instance().LogError
#define RGE_LogDebug		RGE::Logging::LogManager::Instance().LogDebug


class KERNEL_API LogManager{
private:
	LogManager();
	LogManager(const LogManager&) = delete;

	LogManager& operator=(const LogManager&) = delete;

public:
	~LogManager();

	static LogManager&	Instance();

//NOTE: filename = logsFolder + name + {".log", ".html", ".xml", ...}
	LogHandle			CreateLog(const std::string& name, LogType logType);
	void				SetLogsFolder(const std::string& path);
	const std::string&	GetLogsFolder() const;

	void				LogMessage(uint32_t messageType, LogHandle log, const char* fmt, ...); 
	void				LogMessage(uint32_t messageType, LogHandle log, std::string& msg);
	void				LogMessage(uint32_t messageType, LogHandle log, Exception::BaseException& e);

	void				LogInfo(LogHandle log, const char* fmt, ...);
	void				LogInfo(LogHandle log, std::string& msg);
	void				LogInfo(LogHandle log, Exception::BaseException& e);

	void				LogWarning(LogHandle log, const char* fmt, ...);
	void				LogWarning(LogHandle log, std::string& msg);
	void				LogWarning(LogHandle log, Exception::BaseException& e);

	void				LogError(LogHandle log, const char* fmt, ...);
	void				LogError(LogHandle log, std::string& msg);
	void				LogError(LogHandle log, Exception::BaseException& e);

	void				LogDebug(LogHandle log, const char* fmt, ...);
	void				LogDebug(LogHandle log, std::string& msg);
	void				LogDebug(LogHandle log, Exception::BaseException& e);
	
	ILog&				Log(LogHandle log);
	const ILog&			Log(LogHandle log) const;
	ILog&				Log(const std::string& name);
	const ILog&			Log(const std::string& name) const;
	LogHandle			GetHandleByName(const std::string& name) const;


	void EnableMessageType(uint32_t messageTypeMask){
		m_messageTypeEnabled |= messageTypeMask;
	}

	void DisableMessageType(uint32_t messageTypeMask){
		m_messageTypeEnabled &= ~messageTypeMask;
	}

	bool IsMessageTypeEnabled(uint32_t messageType) const{
		return (m_messageTypeEnabled & messageType) != 0;
	}

	bool IsMessageTypeDisabled(uint32_t messageType) const{
		return (m_messageTypeEnabled & messageType) == 0;
	}

private:
	mutable Multithreading::RWSpinlock								m_rwLock;

	std::vector< std::pair<std::string, std::shared_ptr<ILog>> >	m_logs;
	std::string														m_logsFolder;
	std::atomic<uint32_t>											m_messageTypeEnabled;
};

}
}