/********************************************************
*	Подключает нужный таймер в зависимости от платформы	*
********************************************************/
#pragma once

#include "../Platform/Settings.h"

#ifdef RGE_WINDOWS
#include "../Platform/Win32/Timer_Win32Impl.h"

typedef RGE::Timing::Win32Timer		Timer;
#endif