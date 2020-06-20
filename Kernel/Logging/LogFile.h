/************************************
*	Класс для вывода лога в файл	*
************************************/
#pragma once

#include "ILog.h"
#include "..\Multithreading\Spinlock.h"
#include <fstream>


namespace RGE
{
namespace Logging
{

class KERNEL_API LogFile : public ILog{
public:
//NOTE: bufferSize не используется в текущей реализации
	LogFile(const std::string& filename, uint32_t bufferSize = 512, uint32_t spinSleep_ms = 5);
	~LogFile();

	void				SetWriteTime(bool b);
	void				SetWriteThreadID(bool b);
	
	void				Write(const char* fmt, ...);
	void				operator<<(const char* str);
	void				operator<<(const std::string& str);
	void				operator<<(const Exception::BaseException& e);

	void				Flush();

	const std::string&	GetFilename() const;

private:
// вывод дополнительной информации (время, поток) перед лог-сообщением 
	void				WritePrefix();

private:
	std::ofstream				m_file;						// NOTE: временное решение
	Multithreading::Spinlock	m_lock;

	std::string					m_filename;

	bool						m_writeTime;
	bool						m_writeThreadID;
};

}
}