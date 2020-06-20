/********************************************
*	Базовый класс для всех возможных логов	*
********************************************/
#pragma once

#include "Logging.h"
#include "..\Exception\Exception.h"
#include <string>


namespace RGE
{
namespace Logging
{

class KERNEL_API ILog{
public:
	virtual ~ILog(){}

	virtual void	SetWriteTime(bool b) = 0;
	virtual void	SetWriteThreadID(bool b) = 0;
	
	virtual void	Flush() = 0;
	virtual void	Write(const char* fmt, ...) = 0;

	virtual void	operator<<(const char* str) = 0;
	virtual void	operator<<(const std::string& str) = 0;
	virtual void	operator<<(const Exception::BaseException& e) = 0;
};

}
}