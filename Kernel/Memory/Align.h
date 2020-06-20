/********************************************************
*	класс, описывающий выравнивание объекта в памяти;	*
*	значение выравнивания должно быть строго больше 0	*
*				и кратно степени двойки					*
********************************************************/
#pragma once

#include "Memory.h"
#include <cstdint>


namespace RGE
{
namespace Memory
{

class Align{
public:
	Align(uint32_t align = 1){
		m_val = align;
	}
	~Align(){}

	uint32_t ComputeAdjustment(const void* ptr) const{
		uint32_t mask			=	m_val - 1;
		uint32_t misalignment	=	reinterpret_cast<uint32_t>(ptr) & mask;

		if(misalignment == 0)	
			return 0;
		return m_val - misalignment;
	}

	void* GetAligned(void* ptr) const{
		return reinterpret_cast<uint8_t*>(ptr) + ComputeAdjustment(ptr);
	}

public:
	uint32_t	m_val;
};

}
}