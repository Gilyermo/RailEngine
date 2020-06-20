/****************************************************
*  функции форматирования строк (аналог snprintf)	*
****************************************************/
#pragma once

#include "..\KernelDLL.h"
#include <stdarg.h>
#include <string>


namespace RGE
{
namespace Misc
{

int				KERNEL_API	FormatString(char* buf, size_t bufSize, const char* fmt, ...);
int				KERNEL_API	vFormatString(char* buf, size_t bufSize, const char* fmt, va_list args);

int				KERNEL_API	FormatString(std::string& str, const char* fmt, ...);
int				KERNEL_API	vFormatString(std::string& str, const char* fmt, va_list args);

std::string		KERNEL_API	FormattedString(const char* fmt, ...);

}
}