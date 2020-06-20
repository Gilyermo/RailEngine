/************************************
*	����� ��� ������ ���� � ����	*
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
//NOTE: bufferSize �� ������������ � ������� ����������
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
// ����� �������������� ���������� (�����, �����) ����� ���-���������� 
	void				WritePrefix();

private:
	std::ofstream				m_file;						// NOTE: ��������� �������
	Multithreading::Spinlock	m_lock;

	std::string					m_filename;

	bool						m_writeTime;
	bool						m_writeThreadID;
};

}
}