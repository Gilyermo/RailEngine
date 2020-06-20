/************************************************************
*	������ ������������� ������ �������, �� �� ���������;	*
*			��� ��������� ������ ����������� ������			*
************************************************************/
#pragma once

#include "Memory.h"
#include "IAllocator.h"
#include "Arena.h"
#include "../Multithreading/RWSpinlock.h"


namespace RGE
{
namespace Memory
{

class KERNEL_API LinearAllocator : public IAllocator{
private:
	LinearAllocator(const LinearAllocator&) = delete;
	LinearAllocator& operator=(const LinearAllocator&) = delete;

public:
	LinearAllocator(uint32_t size);
	LinearAllocator(Arena& arena);
	~LinearAllocator();

	void*		Allocate(uint32_t size);
	void*		Allocate(uint32_t size, const Align& align);

	void		Deallocate(void*){}								// �������-��������. ������ ������������� �������� Clear()
	void		Clear();										// ������ ������������� ���������, �� �� ���������

	uint32_t	GetMaximumAllocationSize() const;
	uint32_t	GetFreeMemorySize() const;
	uint32_t	GetUsedMemorySize() const;

	bool		IsMemoryAllocatedHere(const void* ptr) const;

private:
	uint32_t	GetFreeMemorySize_unsafe() const;				// ������������� ������� ��� ����������� �������������

private:
	mutable Multithreading::RWSpinlock	m_rwlock;
	mutable Arena						m_arena;
	uint8_t*							m_top;
};

}
}