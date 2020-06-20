#include "CppUnitTest.h"
#include <Memory\Arena.h>
#include <Memory\LinearAllocator.h>
#include <Memory\StackAllocator.h>
#include <Memory\PoolAllocator.h>
#include <Memory\Adapter.h>
#include <Multithreading\Thread.h>
#include <Math\Matrix44.h>
#include <Math\AABB.h>
#include <vector>


namespace RGE
{
namespace UnitTest
{

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Math;
using namespace Multithreading;
using namespace Memory;


TEST_CLASS(MemoryTest){
public:
	TEST_METHOD(LinearAllocatorTest){
		printf("LinearAllocatorTest:\n");
		LinearAllocator allocator(4096);

		uint8_t* mem1 = static_cast<uint8_t*>(allocator.Allocate(100));
		void* mem2 = allocator.Allocate(10, Align(16));

		uint32_t freeMem = allocator.GetFreeMemorySize();

		allocator.Clear();
		freeMem = allocator.GetFreeMemorySize();
	}

	TEST_METHOD(StackAllocatorTest){
		printf("StackAllocatorTest: ");
		StackAllocator allocator(4096);

		try{
			uint32_t freeMem[5];
			freeMem[0] = allocator.GetFreeMemorySize();

			void* ptr1 = allocator.Allocate(100, 4);
			freeMem[1] = allocator.GetFreeMemorySize();

			void* ptr2 = allocator.Allocate(10 * sizeof(Math::AABB));
			freeMem[2] = allocator.GetFreeMemorySize();

			void* ptr3 = allocator.Allocate(1000);
			freeMem[3] = allocator.GetFreeMemorySize();

			void* ptr4 = allocator.Allocate(2000, Align(16));
			freeMem[4] = allocator.GetFreeMemorySize();


			allocator.Deallocate(ptr4);
			if(freeMem[3] != allocator.GetFreeMemorySize())
				throw Exception::TestFailed("Wrong memory size freed");

			allocator.Deallocate(ptr3);
			if(freeMem[2] != allocator.GetFreeMemorySize())
				throw Exception::TestFailed("Wrong memory size freed");

			allocator.Deallocate(ptr2);
			if(freeMem[1] != allocator.GetFreeMemorySize())
				throw Exception::TestFailed("Wrong memory size freed");

			allocator.Deallocate(ptr1);
			if(freeMem[0] != allocator.GetFreeMemorySize())
				throw Exception::TestFailed("Wrong memory size freed");
		}
		catch(const std::exception& e){
			printf("%s\n", e.what());
			return;
		}
	}

	TEST_METHOD(PoolAllocatorTest){
		struct PoolElem{
			Math::Matrix44	mat;
			uint8_t			garbage;
		};

		printf("PoolAllocatorTest:\n");
		Align align(1);
		PoolAllocator<PoolElem>	pool(100, align);
		PoolElem*				poolElem[101];

		try{
			for(int i = 0; i<101; i++){
				poolElem[i] = new(pool.Allocate()) PoolElem;
				uint32_t adj = align.ComputeAdjustment(poolElem[i]);
				if(adj != 0)
					printf("Error in alignment (%i)\n", i);
			}
		}
		catch(std::exception& e){
			printf("%s\n", e.what());
		}

		printf("Pool extra size = %i, elemCount = %i / %i\n", pool.GetExtraMemorySize(), pool.GetUsedElementsCount(), pool.GetTotalElementsCount());

		try{
			for(int i = 0; i<101; i++){
				poolElem[i]->~PoolElem();
				pool.Deallocate(poolElem[i]);
			}
		}
		catch(std::exception& e){
			printf("%s\n", e.what());
		}
	}


#pragma region HeapAllocatorTest
	TEST_METHOD(HeapAllocatorTest){
	}


	static void PerThreadAllocations(int allocNum){
		struct data{
			void*	mem;
			int		size;
		};

		
	}
#pragma endregion



	TEST_METHOD(NewDeleteTest){
		printf("AdapterTest: ");
		PoolAllocator<Matrix44>		matrixPool(10, Align(8));
		PoolAllocator<Thread>		threadPool(10, Align(8));
		StackAllocator				stack(1024);

		try{
			Matrix44* m1 = CreateNew<Matrix44>(matrixPool, Matrix44(1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 2.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 3.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 4.0f));
			auto m2 = CreateUnique<Matrix44>(stack, Matrix44(0.0f, 0.0f, 1.0f, 5.0f));
			auto th1 = CreateShared<Thread>(threadPool);

			auto deleteTask = th1->Execute([&](){  Delete(matrixPool, m1); });
			deleteTask->GetResult();
			th1.reset();

			auto m2_ptr = m2.get();
			m2.reset();

			RGE_Assert(threadPool.GetUsedElementsCount() == 0, Exception::TestFailed, "CreateShared memory not freed correctly");
			RGE_Assert(stack.GetUsedMemorySize() == 0, Exception::TestFailed, "CreateUnique memory not freed correctly");
			printf("[OK]\n");
		}
		catch(Exception::BaseException& e){
			printf("%s\n", e.What());
		}
	}
};


}
}