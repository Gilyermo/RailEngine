#include "Arena.h"
#include <memory>


namespace RGE
{
namespace Memory
{

Arena::Arena(){
	m_memory = nullptr;
	m_size	 = 0;	
}

Arena::Arena(uint32_t size){
	m_memory = malloc(size);
	m_size	 = size;
}

Arena::Arena(Arena&& arena){
	m_memory = arena.m_memory;
	m_size	 = arena.m_size;

	arena.m_memory	=	nullptr;
	arena.m_size	=	0;
}


Arena::~Arena(){
	Destroy();
}


void* Arena::GetMemoryBegin(){
	return m_memory;
}

void* Arena::GetMemoryEnd(){
	return static_cast<uint8_t*>(m_memory) + m_size;
}

size_t Arena::GetSize() const{
	return m_size;
}


void Arena::Swap(Arena& arena){
	std::swap(arena.m_memory, m_memory);
	std::swap(arena.m_size, m_size);
}

void Arena::Destroy(){
	if(m_memory){
		free(m_memory);		
		m_memory = nullptr;
		m_size = 0;
	}
}


bool Arena::IsPointerFromHere(const void* ptr) const{
	return ptr > m_memory  &&  ptr < static_cast<uint8_t*>(m_memory) + m_size;
}


Arena& Arena::operator=(Arena&& arena){
	if(this != &arena)
		Swap( std::forward<Arena>(arena) );
	return *this;
}


}
}