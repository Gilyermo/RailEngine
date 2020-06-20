/****************************************************************************
*	Выделяет память под один объект типа T с выравниванием align за раз;	*
*			конструктор и деструктор объектов не вызывается; 				*
*			 автоматически расширяется при нехватке памяти;					*
*				при выделении память заполняется нулями;					*
*		структура памяти: | nextArena | adjustment|link/obj | ... |			* 
****************************************************************************/
#pragma once

#include "Memory.h"
#include "Arena.h"
#include "IAllocator.h"
#include "Align.h"
#include "../Multithreading/RWSpinlock.h"
#include <algorithm>


namespace RGE
{
namespace Memory
{

template <class T>
class PoolAllocator : public IAllocator{
public:
	struct Link{
		Link* next;
	};

private:
	PoolAllocator(const PoolAllocator&) = delete;
	PoolAllocator& operator=(const PoolAllocator&) = delete;

public:
//NOTE: реальное количество объектов может быть больше запрошенного
	PoolAllocator(uint32_t count = 32, const Align& align = Align(1)){
		m_chunkSize			=	std::max(sizeof(T), sizeof(Link));
		uint32_t size		=	count * (m_chunkSize + align.m_val-1) + sizeof(Arena*);
		m_fstArena			=	new Arena(size);
		uint32_t realCount	=	InitializeMemory(m_fstArena);
		
		m_align				=	align;
		m_freeCount			=	realCount;
		m_totalCount		=	realCount;
		m_arenaCount		=	1;
	}

	~PoolAllocator(){
		m_arenaCount		=	0;
		m_freeCount			=	0;
		m_totalCount		=	0;
		m_fstFree			=	nullptr;

		while(m_fstArena != nullptr){
			Arena* cur	=	m_fstArena;
			m_fstArena	=	*static_cast<Arena**>(m_fstArena->GetMemoryBegin());	
			delete cur;
		}
	}


//NOTE: входной параметр игнорируется
	void* Allocate(uint32_t count = 1){
		Multithreading::WriteLockGuard	lock(m_rwlock);

		if(m_freeCount == 0)	
			CreateNewArena();

		void* mem = m_fstFree;
		m_fstFree = m_fstFree->next;
		m_freeCount--;
		memset(mem, 0, sizeof(T));	

		return mem;
	}						
	
	void Deallocate(void* ptr){
		if(ptr == nullptr)	return;

		Multithreading::WriteLockGuard	lock(m_rwlock);
		RGE_Assert(IsMemoryAllocatedHere(ptr), Exception::WrongArgument, "Memory wasn't allocated here");

		Link* elem = static_cast<Link*>(ptr);
		elem->next = m_fstFree;
		m_fstFree  = elem;
		m_freeCount++;
	}


	uint32_t GetTotalElementsCount() const{
		Multithreading::ReadLockGuard	lock(m_rwlock);
		return m_totalCount;
	}

	uint32_t GetFreeElementsCount() const{
		Multithreading::ReadLockGuard	lock(m_rwlock);
		return m_freeCount;
	}

	uint32_t GetUsedElementsCount() const{
		Multithreading::ReadLockGuard	lock(m_rwlock);
		return m_totalCount - m_freeCount;
	}

	const Align& GetAlignment() const{
		Multithreading::ReadLockGuard	lock(m_rwlock);
		return m_align;
	}

	uint32_t GetMaximumAllocationSize() const{
		return sizeof(T);
	}

	uint32_t GetFreeMemorySize() const{
		Multithreading::ReadLockGuard	lock(m_rwlock);
		return m_freeCount * sizeof(T);
	}

	uint32_t GetUsedMemorySize() const{
		Multithreading::ReadLockGuard	lock(m_rwlock);
		return m_fstArena->GetSize()*m_arenaCount - m_freeCount*sizeof(T);					// GetTotalMemorySize() - GetFreeMemorySize();
	}
;
	uint32_t GetTotalMemorySize() const{
		Multithreading::ReadLockGuard	lock(m_rwlock);
		return m_fstArena->GetSize() * m_arenaCount;
	}

// дополнительная память под выравнивание и связи
	uint32_t GetExtraMemorySize() const{
		Multithreading::ReadLockGuard	lock(m_rwlock);
		return m_fstArena->GetSize()*m_arenaCount - m_totalCount*sizeof(T);					// GetTotalMemorySize() - m_totalCount*sizeof(T)
	}

	bool IsMemoryAllocatedHere(const void* ptr) const{
		Arena* curArena = m_fstArena;
		while(curArena != nullptr){
			if(curArena->IsPointerFromHere(ptr))
				return true;
			curArena = *static_cast<Arena**>(curArena->GetMemoryBegin());
		}
		return false;
	}

private:
// структура свободной памяти:	|adjust|Link/Object|... (Link занимает часть памяти объекта)
// структура занятой памяти:	|adjust|Object|...
	uint32_t InitializeMemory(Arena* arena){
		uint8_t* mem		=	static_cast<uint8_t*>( arena->GetMemoryBegin() );
		uint8_t* memEnd		=	static_cast<uint8_t*>( arena->GetMemoryEnd() );
		uint16_t adjust		=	m_align.ComputeAdjustment(mem);
		uint32_t offset		=	adjust + m_chunkSize;
		uint32_t realCount	=	0;

		*reinterpret_cast<Arena**>(mem) = nullptr;		// начало каждого блока содержит указатель на следующий блок
		mem += sizeof(Arena*);
		
		Link** curLink	=	&m_fstFree;
		while(mem + offset <= memEnd){
			*curLink	=	reinterpret_cast<Link*>(mem + adjust);
			curLink		=	&( (*curLink)->next );

			mem			+=	offset;
			adjust		=	m_align.ComputeAdjustment(mem);
			offset		=	adjust + m_chunkSize;

			realCount++;
		}
		*curLink = nullptr;

		return realCount;
	}

	void CreateNewArena(){
		Arena* newArena =	new Arena(m_fstArena->GetSize());
		uint32_t count	=	InitializeMemory(newArena);
		m_freeCount		+=	count;
		m_totalCount	+=	count;

		*static_cast<Arena**>( newArena->GetMemoryBegin() ) = m_fstArena;
		m_fstArena = newArena;
		m_arenaCount++;
	}

private:
	Arena*		m_fstArena;
	Link*		m_fstFree;					// первый свободный элемент
	Align		m_align;
	
	uint16_t	m_chunkSize;				// m_chunkSize = max( sizeof(T), sizeof(Link) )
	uint16_t	m_arenaCount;
	uint32_t	m_totalCount;
	uint32_t	m_freeCount;

	mutable Multithreading::RWSpinlock		m_rwlock;
};

}
}