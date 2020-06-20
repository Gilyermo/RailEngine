#pragma once

#include "../KernelDLL.h"


namespace RGE
{
namespace Memory
{

	class				Align;
	class	KERNEL_API	Arena;
	class	KERNEL_API	IAllocator;
	class	KERNEL_API	LinearAllocator;
	class	KERNEL_API	StackAllocator;
	template <class T>		
	class				PoolAllocator;
	class	KERNEL_API	HeapSegment;
	class	KERNEL_API	HeapAllocator;

	template <typename T>					class	Deleter;
	template <class T, class RGEAllocator>	class	STLAllocatorAdapter;

}
}