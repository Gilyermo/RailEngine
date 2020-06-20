#pragma once

#ifdef KERNEL_EXPORTS
#define KERNEL_API	__declspec(dllexport)
#else
#define KERNEL_API	__declspec(dllimport)
#endif

#pragma warning(disable : 4251 4275)