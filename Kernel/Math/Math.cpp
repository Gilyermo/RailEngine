#include "Math.h"

namespace RGE
{
namespace Math
{

float EPS = 1E-4f;


uint8_t HighestOneBit(uint64_t val){
	val |= val >> 1;
	val |= val >> 2;
	val |= val >> 4;
	val |= val >> 8;
	val |= val >> 16;
	val |= val >> 32;
	return static_cast<uint8_t>( val - (val >> 1) );
}

uint8_t HighestOneBit(uint32_t val){
	val |= val >> 1;
	val |= val >> 2;
	val |= val >> 4;
	val |= val >> 8;
	val |= val >> 16;
	return static_cast<uint8_t>( val - (val >> 1) );
}

uint8_t HighestOneBit(uint16_t val){
	val |= val >> 1;
	val |= val >> 2;
	val |= val >> 4;
	val |= val >> 8;
	return static_cast<uint8_t>( val - (val >> 1) );
}

uint8_t HighestOneBit(uint8_t val){
	val |= val >> 1;
	val |= val >> 2;
	val |= val >> 4;
	return val - (val >> 1);
}

}
}