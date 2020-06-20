#pragma once

#include "Memory.h"
#include "../Exception/Exception.h"
#include <cstdint>
#include <memory>


namespace RGE
{
namespace Memory
{

class KERNEL_API IAllocator{
public:
	virtual ~IAllocator(){}

	virtual void*		Allocate(uint32_t size) = 0;
	virtual void		Deallocate(void* ptr) = 0;

	virtual uint32_t	GetMaximumAllocationSize() const = 0;
	virtual uint32_t	GetUsedMemorySize() const = 0;
	virtual uint32_t	GetFreeMemorySize() const = 0;

	virtual bool		IsMemoryAllocatedHere(const void* ptr) const = 0;
};

}
}