/********************************************************************
*			�������� ����� ������ ������������� �������;			*
*	������������ ������ ����������� � �������, �������� ���������;	*
*				����� ���������� ������ ����������;					*
********************************************************************/
#pragma once

#include "IAllocator.h"
#include "Arena.h"
#include "Align.h"
#include "../Multithreading/RWSpinlock.h"


namespace RGE
{
namespace Memory
{

class KERNEL_API StackAllocator : public IAllocator{
private:
	struct AllocationInfo{
#ifndef NDEBUG
		void*		prevAlloc;
#endif
		uint16_t	adjust;
	};

private:
	StackAllocator(const StackAllocator&) = delete;
	StackAllocator& operator=(const StackAllocator&) = delete;

public:
	StackAllocator(uint32_t size);
	StackAllocator(Arena& arena);
	~StackAllocator();

	void*		Allocate(uint32_t size);
	void*		Allocate(uint32_t size, const Align& align);

	void		Deallocate(void* ptr);			
	void		Clear();									// ������ ������������ ������
	
	uint32_t	GetMaximumAllocationSize() const;
	uint32_t	GetFreeMemorySize() const;
	uint32_t	GetUsedMemorySize() const;

	bool		IsMemoryAllocatedHere(const void* ptr) const;

private:
	uint32_t	GetFreeMemorySize_unsafe() const;			// ������������� ������� ��� ����������� �������������

private:
	mutable Multithreading::RWSpinlock	m_rwlock;
	mutable Arena						m_arena;
	uint8_t*							m_top;

#ifndef NDEBUG
	void*								m_lstAlloc;
#endif
};

}
}
