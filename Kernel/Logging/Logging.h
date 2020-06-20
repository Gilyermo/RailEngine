/********************************************************
*	ќбъ€влени€ классов и функций дл€ работы с логами	*
********************************************************/
#pragma once

#include "../KernelDLL.h"
#include <cstdint>


namespace RGE
{
namespace Logging
{
	
	class KERNEL_API	ILog;
	class KERNEL_API	LogFile;
	class KERNEL_API	LogManager;

	typedef int32_t		LogHandle;

	enum LogType{
		LT_File = 1,
		//LT_Console,					//NOTE: Not implemented
		//LT_HTML,						//NOTE: Not implemented
		LT_Count
	};

	enum MessageType {
		MT_Info		=	1 << 0,
		MT_Warning	=	1 << 1,
		MT_Error	=	1 << 2,
		MT_Debug	=	1 << 3
	};

}
}