/****************************************************
*	ќбъ€влени€ всех классов дл€ работы с потоками	*
****************************************************/
#pragma once

#include "..\KernelDLL.h"


namespace RGE
{
namespace Multithreading
{
//==================
//		Locks
//==================
	class				Spinlock;
	class KERNEL_API	IReadWriteLock;
	class KERNEL_API	RWSpinlock;		
	class				ReadLockGuard;
	class				WriteLockGuard;

//==================
//	  Structures
//==================
	template <class Container>	class	SafeContainer;
	template <class T>			class	List;

//==================
//	Threads & Tasks
//==================
	class							ITask;
	template <typename T> 	class	Task; 
	template <typename T>	class	Multitask;
	template <typename T>	class	TaskProxy;
	template <typename T>	class	MultitaskProxy;	
	class KERNEL_API				Thread;
	class KERNEL_API				ThreadManager;

}
}