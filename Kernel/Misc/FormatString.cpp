#include "FormatString.h"
#include "..\Exception\Exception.h"


namespace RGE
{
namespace Misc
{

int FormatString(char* buf, size_t bufSize, const char* fmt, ...){
	va_list args;

	va_start(args, fmt);
	int count = vFormatString(buf, bufSize, fmt, args);
	va_end(args);

	return count;
}

int vFormatString(char* buf, size_t bufSize, const char* fmt, va_list args){
#ifdef _MSC_VER
	return vsnprintf_s(buf, bufSize, bufSize, fmt, args);
#else 
	return vsnprintf(buf, bufSize, fmt, args);
#endif
}


int FormatString(std::string& str, const char* fmt, ...){
	va_list args;

	va_start(args, fmt);
	int count = vFormatString(str, fmt, args);
	va_end(args);

	return count;
}

int vFormatString(std::string& str, const char* fmt, va_list args){
	const size_t bufSize = 4096;
	char buf[bufSize];

	int count = vFormatString(buf, bufSize, fmt, args);
	str = buf;
//TODO: выводить это сообщение в лог с предупреждениями
	RGE_Assert(count>=0 && count<bufSize, Exception::WrongState, "Internal buffer is too small for such format");

	return count;
}

std::string KERNEL_API FormattedString(const char * fmt, ...)
{
	std::string tmp;
	va_list args;

	va_start(args, fmt);
	vFormatString(tmp, fmt, args);
	va_end(args);

	return tmp;
}

}
}