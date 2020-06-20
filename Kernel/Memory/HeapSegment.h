/****************************************************************************************
*	  ��������������� ����� ��� HeapAllocator'a, �� ����� ���� ����������� ��������;	*
*	������������ ����� ����������� ������� ������, ������� � �������� ��������� ������	*
*				����������� �� ������� � �������� ������ �������� �����;				*
*			��� ������ ������������ ������ ���������� ��������� ������������;			*
****************************************************************************************/
#pragma once

#include "Memory.h"
#include "IAllocator.h"
#include "Arena.h"
#include "Align.h"
#include "..\Multithreading\RWSpinlock.h"
#include <mutex>


namespace RGE
{
namespace Memory
{

class KERNEL_API HeapSegment : public IAllocator{
public:
	union ChunkHeader{
		struct FreeChunk{
			void* next;							// ��������� �� ��������� ������� ������ �� �������
		} freeChunk;

		struct UsedChunk{
			HeapSegment*	segmentPtr;			// ��������� �� �������, ������� ������� ������
			uint16_t		adjustValue;		// ������� ���� ������������ ��� ������������
			uint16_t		sizeClass;			// �� ������ ���� �������� ������

			void Set(HeapSegment* s, uint16_t adjust, uint16_t size){
				segmentPtr = s;		adjustValue = adjust;	sizeClass = size;
			} 
		} usedChunk;
	};

private:
	HeapSegment(const HeapSegment&) = delete;
	HeapSegment& operator=(const HeapSegment&) = delete;

public:
//NOTE: ������ ����� �������� ������� ������, ��� ���������, ��� ���������� ����
	HeapSegment(uint32_t size, uint8_t heapOwnerIndx = 0);
	~HeapSegment();

	void*		Allocate(uint32_t size);
	void*		Allocate(uint32_t size, const Align& align);
	void		Deallocate(void* ptr);

	uint32_t	GetMaximumAllocationSize() const;		// ������������ ����� ������, ������� ����� �������� �� ���
	uint32_t	GetUnmanagedMemorySize() const;			// ����� ������������������� ������
	uint32_t	GetFreeMemorySize() const;				// ����� ������, ������� ����� ��������
	uint32_t	GetUsedMemorySize() const;

	bool		IsMemoryAllocatedHere(const void* ptr) const;

	uint32_t	GetSizeOfClass(uint8_t sizeClass) const;

	void		SetHeapOwner(uint16_t heapIndx);
	uint16_t	GetHeapOwner() const;
	
private:
	void		Clear();								// ������� ������ �������� �������������������; ����� ������� ����� ���������� ������������ ������

private:
	mutable Arena						m_arena;
	mutable Multithreading::RWSpinlock	m_rwlock;

	void**								m_bin;				// ������ ���������� �� ������� � ���������� �������� ( sizeof(m_bin[i]) = 2^i ) 
	uint16_t							m_binSize;			// ������� ������ ������ ��� m_bin
	uint8_t*							m_segmentTop;		// ������� � ������� �������� ������ �� ������� �� �������

	uint8_t								m_sizeClassCount;	// ���������� ������� ������������ � ����������� �� ������ ��������
	uint32_t							m_freeMemorySize;	// ����� ��������� ������

	std::atomic<uint16_t>				m_heapOwner;		// ����� ���� ����������� ������� (����������� ���������� HeapAllocator)
};

}
}