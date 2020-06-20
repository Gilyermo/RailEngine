#include "StackAllocator.h"
#include "../Exception/Exception.h"


namespace RGE
{
namespace Memory
{

using namespace Multithreading;

StackAllocator::StackAllocator(uint32_t size){
	m_arena.Swap( Arena(size) );
	m_top = static_cast<uint8_t*>( m_arena.GetMemoryBegin() );

#ifndef NDEBUG
	m_lstAlloc = nullptr;
#endif
}

StackAllocator::StackAllocator(Arena& arena){
	m_arena.Swap(arena);
	m_top = static_cast<uint8_t*>( m_arena.GetMemoryBegin() );

#ifndef NDEBUG
	m_lstAlloc = nullptr;
#endif
}

StackAllocator::~StackAllocator(){
	m_top = nullptr;
#ifndef NDEBUG
	m_lstAlloc = nullptr;
#endif
}


void* StackAllocator::Allocate(uint32_t size){
	return Allocate(size, Align(1));
}

// структура памяти - |adjust|AllocationInfo|RealMemory|
void* StackAllocator::Allocate(uint32_t size, const Align& align){
	WriteLockGuard lock(m_rwlock);

	uint16_t infoSize	=	sizeof(AllocationInfo);
	uint16_t adjust		=	align.ComputeAdjustment(m_top + infoSize); 
	uint16_t totalSize	=	static_cast<uint16_t>( adjust + infoSize + size );

	if(GetFreeMemorySize_unsafe() < totalSize)		
		RGE_Throw(Exception::NotEnoughMemory, "");
	
	void* mem	 =	m_top + adjust + infoSize;
	auto info	 =	reinterpret_cast<AllocationInfo*>(m_top + adjust);
	info->adjust =	adjust;
	memset(mem, 0, size);

#ifndef NDEBUG
	info->prevAlloc	=	m_lstAlloc;
	m_lstAlloc		=	mem;
#endif

	m_top += totalSize;
	return mem;
}


void StackAllocator::Deallocate(void* ptr){
	if(ptr == nullptr)	return;
	
	WriteLockGuard lock(m_rwlock);
	RGE_Assert(m_arena.IsPointerFromHere(ptr) && ptr == m_lstAlloc, 
			   Exception::WrongArgument, "Memory wasn't allocated here or there's wrong sequence of deallocation");

	uint8_t* curPtr	=	static_cast<uint8_t*>(ptr);
	auto info		=	reinterpret_cast<AllocationInfo*>( curPtr - sizeof(AllocationInfo) );
	m_top			=	curPtr - info->adjust - sizeof(AllocationInfo);
#ifndef NDEBUG
	m_lstAlloc		=	info->prevAlloc;
#endif
}

void StackAllocator::Clear(){
	WriteLockGuard lock(m_rwlock);
	m_top = static_cast<uint8_t*>( m_arena.GetMemoryBegin() );
#ifndef NDEBUG
	m_lstAlloc = nullptr;
#endif
}


uint32_t StackAllocator::GetMaximumAllocationSize() const{
	return GetFreeMemorySize();
}

uint32_t StackAllocator::GetFreeMemorySize() const{
	ReadLockGuard lock(m_rwlock);
	return m_arena.GetSize() - GetUsedMemorySize();
}

uint32_t StackAllocator::GetUsedMemorySize() const{
	ReadLockGuard lock(m_rwlock);
	return m_top - static_cast<uint8_t*>( m_arena.GetMemoryBegin() );
}

bool StackAllocator::IsMemoryAllocatedHere(const void * ptr) const{
	return m_arena.IsPointerFromHere(ptr);
}

uint32_t StackAllocator::GetFreeMemorySize_unsafe() const{
	return m_arena.GetSize() - ( m_top - static_cast<uint8_t*>(m_arena.GetMemoryBegin()) );
}

}
}