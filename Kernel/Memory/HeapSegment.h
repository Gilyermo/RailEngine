/****************************************************************************************
*	  Вспомогательный класс для HeapAllocator'a, но может быть использован отдельно;	*
*	представляет собой непрерывный участок памяти, который в процессе выделения памяти	*
*				разбивается на отрезки с размером равным степеням двоек;				*
*			при полном освобождении памяти полученное разбиение уничтожается;			*
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
			void* next;							// указатель на следующий отрезок такого же размера
		} freeChunk;

		struct UsedChunk{
			HeapSegment*	segmentPtr;			// указатель на сегмент, который выделил память
			uint16_t		adjustValue;		// сколько байт использовано для выравнивания
			uint16_t		sizeClass;			// из какого бина выделена память

			void Set(HeapSegment* s, uint16_t adjust, uint16_t size){
				segmentPtr = s;		adjustValue = adjust;	sizeClass = size;
			} 
		} usedChunk;
	};

private:
	HeapSegment(const HeapSegment&) = delete;
	HeapSegment& operator=(const HeapSegment&) = delete;

public:
//NOTE: памяти будет выделено немного больше, чем запрошено, для внутренних нужд
	HeapSegment(uint32_t size, uint8_t heapOwnerIndx = 0);
	~HeapSegment();

	void*		Allocate(uint32_t size);
	void*		Allocate(uint32_t size, const Align& align);
	void		Deallocate(void* ptr);

	uint32_t	GetMaximumAllocationSize() const;		// максимальный объем памяти, который можно выделить за раз
	uint32_t	GetUnmanagedMemorySize() const;			// объем неструктурированной памяти
	uint32_t	GetFreeMemorySize() const;				// объем памяти, который можно выделить
	uint32_t	GetUsedMemorySize() const;

	bool		IsMemoryAllocatedHere(const void* ptr) const;

	uint32_t	GetSizeOfClass(uint8_t sizeClass) const;

	void		SetHeapOwner(uint16_t heapIndx);
	uint16_t	GetHeapOwner() const;
	
private:
	void		Clear();								// сделать память сегмента неструктурированной; будет вызвана после последнего освобождения памяти

private:
	mutable Arena						m_arena;
	mutable Multithreading::RWSpinlock	m_rwlock;

	void**								m_bin;				// массив указателей на отрезки с одинаковым размером ( sizeof(m_bin[i]) = 2^i ) 
	uint16_t							m_binSize;			// сколько памяти отъели под m_bin
	uint8_t*							m_segmentTop;		// начиная с вершины сегмента память не разбита на отрезки

	uint8_t								m_sizeClassCount;	// количество классов определяется в зависимости от объема сегмента
	uint32_t							m_freeMemorySize;	// объем свободной памяти

	std::atomic<uint16_t>				m_heapOwner;		// какой куче принадлежит сегмент (особенности реализации HeapAllocator)
};

}
}