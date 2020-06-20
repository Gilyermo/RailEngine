/********************************************************************************************************
*	�������������� ������ �� ����. ��������� ������ ������ �������. �������� � ������������ �������.	*
*					���������� ������ ���� ��� ��������� ������ � ������ �������;						*
*				   ��� ��������� ������ ��������� ��������� ������ �� ������� ������;					*
*		�� ������ ��������� ������ �������� ��� ������� 8/12 �������������� ���� �� x32/x64 ��������;	*
*				��� �������� ������ ���������� ������� � �������� ������� ������� ��������;				*
********************************************************************************************************/
#pragma once

#include "Memory.h"
#include "IAllocator.h"
#include "PoolAllocator.h"
#include "HeapSegment.h"
#include "../Multithreading/SafeContainer.h"
#include <thread>
#include <list>
#include <vector>


namespace RGE
{
namespace Memory
{

class KERNEL_API HeapAllocator : public IAllocator{
public:
//TODO: �������� �� ����������� ���������� ������
	typedef Multithreading::SafeContainer< std::list<HeapSegment*> >	SegmentList;

	struct Heap{
		SegmentList	segments;
	};

// ��������� ������ ����������� �������� ��� ��������� ������
	enum SearchStrategy{
		FirstFit,				// ������ ������� � ����������� ������� ��������� ������
		BestFit					// ������� � ����������� � ���������� ��������� �������
	};

private:
	HeapAllocator(const HeapAllocator&) = delete;
	HeapAllocator& operator=(const HeapAllocator&) = delete;

public:
// pageSize_kb - ������ �������� � ����������, heapNum - ���������� thread-local ���, 
// ff (free fracture) - ���� ������� ������ ��������, ��� ������� �� �������� �� ���������� ����� ����  
	HeapAllocator(uint32_t pageSize_kb = 16, uint16_t heapNum = std::thread::hardware_concurrency(), float ff = 0.1f, SearchStrategy strategy = BestFit);
	~HeapAllocator();

	void*			Allocate(uint32_t size);
	void*			Allocate(uint32_t size, const Align& align);
	void			Deallocate(void* ptr);

	uint32_t		GetMaximumAllocationSize() const;
	uint32_t		GetFreeMemorySize() const;
	uint32_t		GetUsedMemorySize() const;

	bool			IsMemoryAllocatedHere(const void* ptr) const;

	void			SetSegmentSearchingStrategy(SearchStrategy strategy);
	SearchStrategy	GetSegmentSearchingStrategy() const;

private:
	uint8_t			Hash(std::thread::id& tid) const;		// ���-������� ��� ����������� ������ ���� �� id ������

	HeapSegment*	FindAppropriateSegment(uint16_t heapIndx, uint32_t size);
	HeapSegment*	FindFirstFitSegment(const Heap& h, uint32_t size) const;
	HeapSegment*	FindBestFitSegment(const Heap& h, uint32_t size) const;
	HeapSegment*	AddNewSegment(uint16_t heapIndx, uint32_t size);
	void			AddSegment(uint16_t heapIndx, HeapSegment* seg);

private:
	PoolAllocator<HeapSegment>	m_segPool;
	std::vector<Heap>			m_heaps;			// m_heaps[0] - ����� ����, ��������� - thread-local ���� 
	uint32_t					m_pageSize;			// ������ �������� � ������
	float						m_freeFracture;		// ���� ������� ������ ��������, ��� ������� �� ������������ � ����� ����
	SearchStrategy				m_strategy;
};

}
}