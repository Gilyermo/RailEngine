#include "LinearAllocator.h"
#include "Align.h"


namespace RGE
{
namespace Memory
{

using namespace Multithreading;

LinearAllocator::LinearAllocator(uint32_t size){
	m_arena.Swap( Arena(size) );
	m_top = static_cast<uint8_t*>( m_arena.GetMemoryBegin() );
}

LinearAllocator::LinearAllocator(Arena& arena){
	m_arena.Swap(arena);
	m_top = static_cast<uint8_t*>( m_arena.GetMemoryBegin() );
}

LinearAllocator::~LinearAllocator(){
	m_top = nullptr;
}


void* LinearAllocator::Allocate(uint32_t size){
	return Allocate(size, Align(1));
}

void* LinearAllocator::Allocate(uint32_t size, const Align& align){
	WriteLockGuard lock(m_rwlock);

	uint8_t* mem = m_top;
	uint16_t adj = align.ComputeAdjustment(mem);

	if(GetFreeMemorySize_unsafe() < size + adj)
		RGE_Throw(Exception::NotEnoughMemory, "");

	mem += adj;
	memset(mem, 0, size);
	m_top += size + adj;
	return mem;
}


void LinearAllocator::Clear(){
	WriteLockGuard lock(m_rwlock);
	m_top = static_cast<uint8_t*>( m_arena.GetMemoryBegin() );
}


uint32_t LinearAllocator::GetMaximumAllocationSize() const{
	return GetFreeMemorySize();
}

uint32_t LinearAllocator::GetFreeMemorySize() const{
	ReadLockGuard lock(m_rwlock);
	return m_arena.GetSize() - GetUsedMemorySize();
}

uint32_t LinearAllocator::GetUsedMemorySize() const{
	ReadLockGuard lock(m_rwlock);
	return m_top - static_cast<uint8_t*>( m_arena.GetMemoryBegin() );
}


bool LinearAllocator::IsMemoryAllocatedHere(const void* ptr) const{
	return m_arena.IsPointerFromHere(ptr);
}


uint32_t LinearAllocator::GetFreeMemorySize_unsafe() const{
	return m_arena.GetSize() - ( m_top - static_cast<uint8_t*>(m_arena.GetMemoryBegin()) );
}

}
}