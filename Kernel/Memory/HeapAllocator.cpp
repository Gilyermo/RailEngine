#include "HeapAllocator.h"
#include "Adapter.h"
#include "..\Math\Math.h"


namespace RGE
{
namespace Memory
{

HeapAllocator::HeapAllocator(uint32_t pageSize_kb, uint16_t heapNum, float ff, HeapAllocator::SearchStrategy strategy) 
	: m_pageSize(pageSize_kb * 1024),
	m_freeFracture(ff),
	m_strategy(strategy)
{
	m_heaps.resize(heapNum + 1);
}

HeapAllocator::~HeapAllocator(){
	for(Heap& h : m_heaps){
		while(!h.segments.empty()){
			HeapSegment* seg;
			if(h.segments.try_get_and_pop_back(&seg))
				Delete(m_segPool, seg);
		}
	}
}


void* HeapAllocator::Allocate(uint32_t size){
	return Allocate(size, Align(1));
}

void* HeapAllocator::Allocate(uint32_t size, const Align& align){
	uint8_t heapIndx = Hash(std::this_thread::get_id());
	HeapSegment* seg = FindAppropriateSegment(heapIndx, size + align.m_val - 1);
	return seg->Allocate(size, align);
}

void HeapAllocator::Deallocate(void* ptr){
// смещаемся к заголовку чанка освобождаемой памяти
	HeapSegment::ChunkHeader* chunkHead = static_cast<HeapSegment::ChunkHeader*>(ptr) - 1;
	HeapSegment* seg = chunkHead->usedChunk.segmentPtr;
	seg->Deallocate(ptr);

// если сегмент достаточно свободен, можем вернуть его нулевой куче
	float fracture = seg->GetUsedMemorySize() / static_cast<float>( seg->GetFreeMemorySize() );
	if(fracture < m_freeFracture){
		uint16_t heapIndx = seg->GetHeapOwner();
		m_heaps[heapIndx].segments.try_erase(seg);
		AddSegment(0, seg);
	}
}


uint32_t HeapAllocator::GetMaximumAllocationSize() const{
	return -1;
}

uint32_t HeapAllocator::GetFreeMemorySize() const{
	uint32_t freeMem = 0;

	for(const Heap& h : m_heaps){
		h.segments.for_each([&](HeapSegment* seg)->bool{
			freeMem += seg->GetFreeMemorySize();
			return true;
		});
	}

	return freeMem;
}

uint32_t HeapAllocator::GetUsedMemorySize() const{
	uint32_t usedMem = 0;

	for(const Heap& h : m_heaps){
		h.segments.for_each([&](HeapSegment* seg)->bool{
			usedMem += seg->GetUsedMemorySize();
			return true;
		});
	}

	return usedMem;
}


bool HeapAllocator::IsMemoryAllocatedHere(const void* ptr) const{
	for(const Heap& h : m_heaps){
		bool res;
		h.segments.for_each([&](HeapSegment* seg)->bool{
			if(seg->IsMemoryAllocatedHere(ptr)){
				res = true;
				return false;
			}
			return true;
		});

		if(res)	return true;
	}

	return false;
}


void HeapAllocator::SetSegmentSearchingStrategy(SearchStrategy strategy){
	m_strategy = strategy;
}

HeapAllocator::SearchStrategy HeapAllocator::GetSegmentSearchingStrategy() const{
	return m_strategy;
}


//TODO: 
uint8_t HeapAllocator::Hash(std::thread::id& tid) const{
	return 1 + static_cast<uint8_t>(std::hash<std::thread::id>()(tid) % (m_heaps.size() - 1));
}


HeapSegment* HeapAllocator::FindAppropriateSegment(uint16_t heapIndx, uint32_t size){
	HeapSegment* res = nullptr;

	switch(m_strategy){
	case FirstFit:
		res = FindFirstFitSegment(m_heaps[heapIndx], size);
		break;

	case BestFit:
		res = FindBestFitSegment(m_heaps[heapIndx], size);
		break;
	}

	if(res == nullptr)
		res = AddNewSegment(heapIndx, size);
	return res;
}

HeapSegment* HeapAllocator::FindFirstFitSegment(const Heap& h, uint32_t size) const{
	HeapSegment* fstFitSeg = nullptr;
	h.segments.for_each([&](HeapSegment* seg)->bool{
		if(seg->GetMaximumAllocationSize() >= size){
			fstFitSeg = seg;
			return false;
		}
		return true;
	});

	return fstFitSeg;
}

HeapSegment* HeapAllocator::FindBestFitSegment(const Heap& h, uint32_t size) const{
	HeapSegment* bstFitSeg = nullptr;

	h.segments.for_each([&](HeapSegment* seg)->bool{
		static uint32_t minAllocationSize = -1;
		uint32_t curAllocationSize = seg->GetMaximumAllocationSize();

		if(curAllocationSize >= size){
			if(bstFitSeg && minAllocationSize > curAllocationSize){
				minAllocationSize	=	curAllocationSize;
				bstFitSeg			=	seg;
			}
			else
				bstFitSeg = seg;
		}
		return true;
	});

	return bstFitSeg;
}

HeapSegment* HeapAllocator::AddNewSegment(uint16_t heapIndx, uint32_t size){
	HeapSegment* newSegment = nullptr;
	m_heaps[0].segments.remove_first_if([&](HeapSegment* seg)->bool{
		if(seg->GetMaximumAllocationSize() > size){
			newSegment = seg;
			return true;
		}
		return false;
	});

	if(newSegment == nullptr){
		uint32_t segSize = Math::Ceil(size, m_pageSize) * m_pageSize;
		newSegment = CreateNew<HeapSegment>(m_segPool, segSize);
		AddSegment(heapIndx, newSegment);
	}

	return newSegment;
}

void HeapAllocator::AddSegment(uint16_t heapIndx, HeapSegment* seg){
	m_heaps[heapIndx].segments.push_front(seg);
	seg->SetHeapOwner(heapIndx);
}

}
}