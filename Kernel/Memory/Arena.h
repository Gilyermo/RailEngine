/************************************************************
*	непрерывный блок памяти, которым управляют аллокаторы	*
************************************************************/
#pragma once

#include "Memory.h"
#include <cstdint>


namespace RGE
{
namespace Memory
{

class KERNEL_API Arena{
public:
	Arena();
	explicit Arena(uint32_t size);
	Arena(Arena& arena) = delete;
	Arena(Arena&& arena);
	~Arena();

	void*		GetMemoryBegin();
	void*		GetMemoryEnd();
	uint32_t	GetSize() const;

	void		Swap(Arena& arena);
	void		Destroy();

	bool		IsPointerFromHere(const void* ptr) const;

	Arena&		operator=(Arena&& arena);

private:
	void*		m_memory;		// блок памяти
	uint32_t	m_size;			// размер блока в байтах
};

}
}