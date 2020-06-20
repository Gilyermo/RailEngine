#include "HeapSegment.h"
#include "../Math/Math.h"
#include "../Exception/Exception.h"
#include <algorithm>


namespace RGE
{
namespace Memory
{

HeapSegment::HeapSegment(uint32_t size, uint8_t heapOwnerIndex) 
	: m_heapOwner(heapOwnerIndex)
{
	m_sizeClassCount = Math::HighestOneBit(size) + 1;											
	m_binSize = m_sizeClassCount*sizeof(void*);
	size = std::max<uint32_t>( size, GetSizeOfClass(m_sizeClassCount-1) + sizeof(ChunkHeader::UsedChunk) );	// если запросят максимум памяти, ее должно хватить и под заголовок

	m_arena.Swap( Arena(size + m_binSize) );
	*m_bin = m_arena.GetMemoryBegin();
	memset(*m_bin, 0, m_binSize);

	m_segmentTop		=	static_cast<uint8_t*>(*m_bin) + m_binSize;
	m_freeMemorySize	=	size;
}

HeapSegment::~HeapSegment(){
	m_segmentTop		=	nullptr;
	m_bin				=	nullptr;
	m_freeMemorySize	=	0;
	m_sizeClassCount	=	0;
}


void * HeapSegment::Allocate(uint32_t size){
	return Allocate(size, Align(1));
}

void* HeapSegment::Allocate(uint32_t size, const Align& align){
	size += align.m_val-1;
	if(GetMaximumAllocationSize() < size)
		RGE_Throw(Exception::NotEnoughMemory, "");

	uint8_t headerSize	=	sizeof(ChunkHeader);
	uint8_t sizeClass	=	Math::HighestOneBit(size-1) + 1;					// Log2(size-1) + 1
	uint32_t chunkSize	=	GetSizeOfClass(sizeClass) + headerSize;
	uint8_t* mem;

	Multithreading::WriteLockGuard lock(m_rwlock);

// если ранее выделенных свободных кусков нет, но есть нераспределенная память - пытаемся взять из нее
	if(m_bin[sizeClass] == nullptr && GetUnmanagedMemorySize() >= chunkSize){				
		mem	= m_segmentTop;
		m_segmentTop +=	chunkSize;
	}
	else{																		// иначе ищем среди всех свободных кусков
		while(m_bin[sizeClass] == nullptr) 
			sizeClass++;					
		chunkSize = GetSizeOfClass(sizeClass) + headerSize;

		mem = static_cast<uint8_t*>( m_bin[sizeClass] );
		m_bin[sizeClass] = reinterpret_cast<ChunkHeader*> (mem)->freeChunk.next;
	}
	
	uint16_t adjust		=	align.ComputeAdjustment(mem + headerSize);
	ChunkHeader* head	=	reinterpret_cast<ChunkHeader*>( mem + adjust );		//NOTE: смещение для выравнивания добавляется перед заголовком
	head->usedChunk.Set(this, adjust, sizeClass);
	
	m_freeMemorySize -= chunkSize; 
	return mem + adjust + headerSize;
}

void HeapSegment::Deallocate(void* ptr){
	if(ptr == nullptr) 
		return;
	RGE_Assert(IsMemoryAllocatedHere(ptr), Exception::WrongArgument, "Memory wasn't allocated here");

	uint8_t* mem		=	static_cast<uint8_t*>(ptr) - sizeof(ChunkHeader);
	ChunkHeader* head	=	reinterpret_cast<ChunkHeader*>(mem);
	uint16_t adjust		=	head->usedChunk.adjustValue;	
	uint8_t sizeClass	=	static_cast<uint8_t>( head->usedChunk.sizeClass );
	mem -= adjust;

	Multithreading::WriteLockGuard lock(m_rwlock);

	reinterpret_cast<ChunkHeader*>(mem)->freeChunk.next = m_bin[sizeClass];
	m_bin[sizeClass] = mem;
	m_freeMemorySize += GetSizeOfClass(sizeClass) + sizeof(ChunkHeader); 

	if(GetUsedMemorySize() == 0)
		Clear();
}


uint32_t HeapSegment::GetMaximumAllocationSize() const{
	Multithreading::ReadLockGuard lock(m_rwlock);

	uint8_t unmanagedLog2		=	Math::HighestOneBit( GetUnmanagedMemorySize() - sizeof(ChunkHeader) );
	uint8_t maxAvailableClass	=	m_sizeClassCount-1;

	while(m_bin[maxAvailableClass] == nullptr)
		maxAvailableClass--;

	return std::min( std::max<uint32_t>( 1<<unmanagedLog2, GetSizeOfClass(maxAvailableClass) ), 
					 GetSizeOfClass(m_sizeClassCount-1) );
}

uint32_t HeapSegment::GetUnmanagedMemorySize() const{
	return static_cast<uint8_t*>(m_arena.GetMemoryEnd()) - m_segmentTop;
}

uint32_t HeapSegment::GetFreeMemorySize() const{
	return m_freeMemorySize;
}

uint32_t HeapSegment::GetUsedMemorySize() const{
	return m_arena.GetSize() - m_freeMemorySize - m_binSize;
}

bool HeapSegment::IsMemoryAllocatedHere(const void* ptr) const{
	return m_arena.IsPointerFromHere(ptr);
}


uint32_t HeapSegment::GetSizeOfClass(uint8_t sizeClass) const{
	return 1 << sizeClass;
}


void HeapSegment::SetHeapOwner(uint16_t heapIndx){
	m_heapOwner = heapIndx;
}

uint16_t HeapSegment::GetHeapOwner() const{
	return m_heapOwner;
}


void HeapSegment::Clear(){
	memset(*m_bin, 0, m_binSize);
	m_segmentTop = static_cast<uint8_t*>(m_arena.GetMemoryBegin()) + m_binSize;
}

}
}