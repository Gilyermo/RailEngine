/********************************************************************************************************
*	Распределитель памяти на куче. Выделение памяти любого размера. Удаление в произвольном порядке.	*
*					Использует разные кучи для выделения памяти в разных потоках;						*
*				   при выделении памяти округляет требуемый размер до степени двойки;					*
*		на каждое выделение памяти тратится как минимум 8/12 дополнительных байт на x32/x64 системах;	*
*				при нехватке памяти выделяется сегмент с размером кратным размеру страницы;				*
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
//TODO: заменить на собственную реализацию списка
	typedef Multithreading::SafeContainer< std::list<HeapSegment*> >	SegmentList;

	struct Heap{
		SegmentList	segments;
	};

// стратегия поиска подходящего сегмента для выделения памяти
	enum SearchStrategy{
		FirstFit,				// первый сегмент с достаточным объемом свободной памяти
		BestFit					// сегмент с достаточным и минимально возможным объемом
	};

private:
	HeapAllocator(const HeapAllocator&) = delete;
	HeapAllocator& operator=(const HeapAllocator&) = delete;

public:
// pageSize_kb - размер страницы в килобайтах, heapNum - количество thread-local куч, 
// ff (free fracture) - доля занятой памяти сегмента, при которой он отдается на управление общей куче  
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
	uint8_t			Hash(std::thread::id& tid) const;		// хэш-функция для определения номера кучи по id потока

	HeapSegment*	FindAppropriateSegment(uint16_t heapIndx, uint32_t size);
	HeapSegment*	FindFirstFitSegment(const Heap& h, uint32_t size) const;
	HeapSegment*	FindBestFitSegment(const Heap& h, uint32_t size) const;
	HeapSegment*	AddNewSegment(uint16_t heapIndx, uint32_t size);
	void			AddSegment(uint16_t heapIndx, HeapSegment* seg);

private:
	PoolAllocator<HeapSegment>	m_segPool;
	std::vector<Heap>			m_heaps;			// m_heaps[0] - общая куча, остальные - thread-local кучи 
	uint32_t					m_pageSize;			// размер страницы в байтах
	float						m_freeFracture;		// доля занятой памяти сегмента, при которой он возвращается в общую кучу
	SearchStrategy				m_strategy;
};

}
}