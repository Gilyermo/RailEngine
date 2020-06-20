/****************************************************************
*					Набор классов исключений;					* 
*	содержат текст описания ошибки и места, где она возникла	*
****************************************************************/
#pragma once

#include "../KernelDLL.h"
#include <exception>
#include <string>
#include <cstdint>

// 'exception specification is ignored' warning disable
#pragma warning(disable : 4290)


namespace RGE
{
namespace Exception
{

#ifndef NDEBUG	
	#define RGE_Assert(expr, ExceptionType, desc)		if(!(expr))																\
															throw ExceptionType( std::string("Assertion failed: ") +			\
																				 std::string(#expr) + std::string(". ") +		\
																				 desc,											\
																				__FILE__, __LINE__)
#else
	#define RGE_Assert(expr, ExceptionType, desc)	
#endif														
	
#define RGE_Throw(ExceptionType, desc)					throw ExceptionType(desc, __FILE__, __LINE__)


class KERNEL_API BaseException : public std::exception{
public:
	BaseException(const std::string& type = "Base", const std::string& desc = "", 
				  const std::string& file = __FILE__, uint32_t line = __LINE__);
	BaseException(const BaseException& e);
	virtual ~BaseException(){}

	virtual	std::string&		GetDescription();			// позволяет добавлять информацию
	virtual const std::string&	GetDescription() const;
	virtual const std::string&	GetType() const;
	virtual const std::string&	GetFile() const;
	virtual const uint32_t		GetLine() const;

	virtual const char* What() const throw();
	virtual const char* what() const throw();				// исключительно ради совпадения с std::exception; идентична What()

	BaseException& operator=(const BaseException& e) throw();

protected:
	mutable std::string		m_what;			// описание исключения
	std::string				m_desc;			// описание ошибки
	std::string				m_type;			// тип исключения
	std::string				m_file;			// файл, в котором брошено исключение
	uint32_t				m_line;			// номер строки выброса
};


class Undefined : public BaseException{
public:
	Undefined(const std::string& desc = "", const std::string& file = __FILE__, uint32_t line = __LINE__)
		: BaseException("Undefined", desc, file, line)
	{}
};


class TestFailed : public BaseException{
public:
	TestFailed(const std::string& desc = "", const std::string& file = __FILE__, uint32_t line = __LINE__)
		: BaseException("TestFailed", desc, file, line)
	{}
};


class NotEnoughMemory : public BaseException{
public:
	NotEnoughMemory(const std::string& desc = "", const std::string& file = __FILE__, uint32_t line = __LINE__)
		: BaseException("NotEnoughMemory", desc, file, line)
	{}
};


class WrongArgument : public BaseException{
public:
	WrongArgument(const std::string& desc = "", const std::string& file = __FILE__, uint32_t line = __LINE__)
		: BaseException("WrongArgument", desc, file, line)
	{}
};


class WrongState : public BaseException{
public:
	WrongState(const std::string& desc = "", const std::string& file = __FILE__, uint32_t line = __LINE__)
		: BaseException("WrongState", desc, file, line)
	{}
};


class NotImplemented : public BaseException{
public:
	NotImplemented(const std::string& desc = "", const std::string& file = __FILE__, uint32_t line = __LINE__)
		: BaseException("NotImplemented", desc, file, line)
	{}
};

}
}