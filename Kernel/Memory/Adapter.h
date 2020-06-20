/************************************************************
*		набор функций и конструкций для использования		*
*	 RGE аллокаторов в привычном и более безопасном виде	*
************************************************************/
#pragma once

#include "Memory.h"
#include "IAllocator.h"
#include <utility>


namespace RGE
{
namespace Memory
{

//===============================
//			Deleter	
//===============================
template <typename T>
class Deleter {
public:
	Deleter() {
		m_allocator = nullptr;
	}
	Deleter(IAllocator* allocator) {
		m_allocator = allocator;
	}
	Deleter(const Deleter& del) {
		m_allocator = del.m_allocator;
	}
	~Deleter() {
		m_allocator = nullptr;
	}

	void operator()(void* ptr) {
		RGE_Assert(m_allocator && m_allocator->IsMemoryAllocatedHere(ptr), Exception::WrongArgument, "The object wasn't allocated with this allocator");
		static_cast<T*>(ptr)->~T();
		m_allocator->Deallocate(ptr);
	}

private:
	IAllocator*		m_allocator;
};


//===============================
//		New / Delete analog
//===============================
template <class T, class... Types>
T* CreateNew(IAllocator& allocator, Types... ctorArgs) {
	T* obj = static_cast<T*>(allocator.Allocate(sizeof(T)));
	new(obj) T(ctorArgs ...);
	return obj;
}

template <class T>
T* CreateNew(IAllocator& allocator) {
	T* obj = static_cast<T*>(allocator.Allocate(sizeof(T)));
	new(obj) T();
	return obj;
}

template <class T>
T* CreateArray(IAllocator& allocator, uint32_t count) {
	return static_cast<T*>(allocator.Allocate(count * sizeof(T)));
}

template <class T>
void Delete(IAllocator& allocator, T* obj) {
	RGE_Assert(allocator.IsMemoryAllocatedHere(obj), Exception::WrongArgument, "The object wasn't allocated with this allocator");
	obj->~T();
	allocator.Deallocate(obj);
}


template <class T, class... Types>
std::unique_ptr< T, Deleter<T> > CreateUnique(IAllocator& allocator, Types... ctorArgs) {
	std::unique_ptr< T, Deleter<T> > obj(static_cast<T*>(allocator.Allocate(sizeof(T))), Deleter<T>(&allocator));
	new(obj.get()) T(ctorArgs ...);
	return obj;
}

template <class T>
std::unique_ptr< T, Deleter<T> > CreateUnique(IAllocator& allocator) {
	std::unique_ptr< T, Deleter<T> > obj(static_cast<T*>(allocator.Allocate(sizeof(T))), Deleter<T>(&allocator));
	new(obj.get()) T();
	return obj;
}


template <class T, class... Types>
std::shared_ptr<T> CreateShared(IAllocator& allocator, Types... ctorArgs) {
	std::shared_ptr<T> obj(static_cast<T*>(allocator.Allocate(sizeof(T))), Deleter<T>(&allocator));	//TODO: здесь все равно используется new для внутренних данных shared_ptr
	new(obj.get()) T(ctorArgs ...);
	return obj;
}

template <class T>
std::shared_ptr<T> CreateShared(IAllocator& allocator) {
	std::shared_ptr<T> obj(static_cast<T*>(allocator.Allocate(sizeof(T))), Deleter<T>(&allocator)); //TODO: здесь все равно используется new для внутренних данных shared_ptr
	new(obj.get()) T();
	return obj;
}


//===============================
//	adapter to std::allocator 
//===============================
template <class T, class RGEAllocator>
class STLAllocatorAdapter {
public:
	typedef	T			value_type;
	typedef T*			pointer;
	typedef T&			reference;
	typedef const T*	const_pointer;
	typedef const T&	const_reference;
	typedef	size_t		size_type;
	typedef ptrdiff_t	difference_type;

	template <class U> struct rebind {
		typedef STLAllocatorAdapter<U, RGEAllocator> other;
	};

public:
	STLAllocatorAdapter(RGEAllocator* allocator) {
		m_alloc = allocator;
	}

	STLAllocatorAdapter(const STLAllocatorAdapter& adapter) {
		m_alloc = adapter.m_alloc;
	}

	template <class U>
	STLAllocatorAdapter(const STLAllocatorAdapter<U, RGEAllocator>& otherAdapter) {
		m_alloc = otherAdapter.m_alloc;
	}

	virtual ~STLAllocatorAdapter() {
		m_alloc = nullptr;
	}


	pointer allocate(size_type n, const void* hint = nullptr) {
		return m_alloc->Allocate(n * sizeof(T));
	}

	void deallocate(pointer p, size_type n) {
		m_alloc->Deallocate(p);
	}


	template <class U, class... Types>
	void construct(U* p, Types&&... args) {
		new(p) U(std::forward<Types>(args)...);
	}

	template <class U>
	void destroy(U* p) {
		p->~U();
	}


	size_type max_size() const {
		return m_alloc->GetMaximumAllocationSize() / sizeof(T);
	}


	pointer address(reference x) const {
		return std::addressof(x);
	}

	const_pointer address(const_reference x) const {
		return std::addressof(x);
	}

	STLAllocatorAdapter& operator=(const STLAllocatorAdapter& rhs) {
		m_alloc = rhs.m_alloc;
		return *this;
	}

protected:
	IAllocator*	m_alloc;
};


//NOTE: следующие аллокаторы не очень хорошо вписываются в концепцию std::allocator, поэтому их следует использовать с осторожностью
template <class T> using LinearAllocatorAdapter = STLAllocatorAdapter<T, LinearAllocator>;
template <class T> using StackAllocatorAdapter	= STLAllocatorAdapter<T, StackAllocator>;
template <class T> using PoolAllocatorAdapter	= STLAllocatorAdapter<T, PoolAllocator<T>>;

template <class T> using HeapAllocatorAdapter	= STLAllocatorAdapter<T, HeapAllocator>;

}
}