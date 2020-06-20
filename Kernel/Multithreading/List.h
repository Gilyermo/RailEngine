/************************************************
*				!!!! DELAYED !!!!				*
*	потоко-безопасный двунаправленный список	*
*	 с хорошо-гранулированными блокировками		*
************************************************/
#pragma once

#include "Multithreading.h"
#include "Spinlock.h"
#include "RWSpinlock.h"
#include "..\Memory\Adapter.h"
#include "..\Logging\LogManager.h"
#include <atomic>
#include <mutex>
#include <random>
#include <algorithm>


namespace RGE
{
namespace Multithreading
{

template <class T>
class List{
public:
	template <class U>
	struct Node{
		Node(){
			next = prev = nullptr;
			lock.SetSleepTime(5);
			deleteThis.store(false);
			refCount.store(0);
		}

		U							object;
		Node*						next;
		Node*						prev;
		mutable RWSpinlock			lock;
		mutable std::atomic_bool	deleteThis;
		mutable std::atomic_int		refCount;
	};

	class ElementRef{
		friend class List;

	public:
		ElementRef() : m_node(nullptr){}
		ElementRef(Node<T>* node) : m_node(node){}
		ElementRef(const ElementRef& ref) : m_node(ref.m_node){
			this->AddRef();
		}
		ElementRef(ElementRef&& ref) : m_node(ref.m_node){
			ref.m_node = nullptr;
		}
		~ElementRef(){
			this->Release();
		}


		void Release(){
			if(m_node){
				m_node->refCount--;
				m_node = nullptr;
			}
		}

		void Reset(Node<T>* node){
			this->Release();
			m_node = node;
			AddRef();
		}


		T& operator->(){ return m_node->object; }
		const T& operator->() const{ return m_node->object; }
		bool operator()() const{ return m_node != nullptr; }

		ElementRef& operator=(const ElementRef& ref){
			if(this != &ref){
				this->Release();
				m_node = ref.m_node;
				AddRef();
			}
			return *this;
		}

	private:
		void AddRef(){
			if(m_node)
				m_node->refCount++;
		}

	private:
		Node<T>*	m_node;
	};

public:
	List(){
		RGE_Throw(Exception::NotImplemented, "Implementation of RGE::Multithreading::List is delayed. Use SafeConatiner<std::list> instead");

		m_end = Memory::CreateNew<Node<T>>(m_pool);
		m_front = m_end;
		m_back = m_end;

		m_maxSleepTime = 5;
		m_globalLock.set_sleep_time(5);

		m_pushLogHandle = Logging::LogManager::Instance().CreateLog("ListDebug_push", Logging::LT_File);
		m_popLogHandle = Logging::LogManager::Instance().CreateLog("ListDebug_pop", Logging::LT_File);
	}

	~List(){
		while(!empty())
			pop_front();
		Memory::Delete(m_pool, m_end);
	}

	//TODO:
	ElementRef find_if(std::function<bool(const T&)> pred){
	}
	bool remove(ElementRef& ref){
	}
	void for_each(std::function<void(const T&)> oper){
	}

	void push_back(const T& val){
		Node<T>* newNode = Memory::CreateNew<Node<T>>(m_pool);
		newNode->object = val;
		newNode->lock.LockWrite();

		std::lock_guard<Spinlock> lock(m_globalLock);

		// взятие блокировки на запись для m_end и m_back
		while(true){
			if(m_end->lock.TryLockWrite()){
				if(!empty() && !m_back.load()->lock.TryLockWrite()){
					m_end->lock.UnlockWrite();
				}
				else break;
			}

			std::this_thread::sleep_for(GenRandomTime(m_maxSleepTime));
		}

		newNode->prev = m_end->prev;
		newNode->next = m_end;
		m_end->prev = newNode;

		if(!empty())	m_back.load()->next = newNode;
		else			m_front = newNode;
		m_back = newNode;
		m_size++;

		// разлочиваем узлы
		if(m_back.load()->prev)
			m_back.load()->prev->lock.UnlockWrite();
		m_back.load()->lock.UnlockWrite();
		m_back.load()->next->lock.UnlockWrite();
	}

	void pop_back(){
		while(!TryLockNodes(m_end, m_back, m_back.load()->prev)){
			if(empty())	return;
			std::this_thread::sleep_for(GenRandomTime(m_maxSleepTime));
		}
		m_size--;

		Node* tmp = m_back;
		m_end->prev = m_back.load()->prev;

		// если удалили последний элемент
		if(empty()){
			m_front = m_end;
			m_back = m_end;
		}
		else{
			m_back = m_back.load()->prev;
			m_back.load()->next = m_end;
			m_end->lock.UnlockWrite();
		}
		m_back.load()->lock.UnlockWrite();

		Memory::Delete(m_pool, tmp);
	}

	bool pop_back(T* val){
		while(!TryLockNodes(m_end, m_back, m_back.load()->prev)){
			if(empty())	return false;
			std::this_thread::sleep_for(GenRandomTime(m_maxSleepTime));
		}
		m_size--;

		Node* tmp = m_back;
		*val = m_back.load()->object;
		m_end->prev = m_back.load()->prev;

		// если удалили последний элемент
		if(empty()){
			m_front = m_end;
			m_back = m_end;
		}
		else{
			m_back = m_back.load()->prev;
			m_back.load()->next = m_end;
			m_end->lock.UnlockWrite();
		}
		m_back.load()->lock.UnlockWrite();

		Memory::Delete(m_pool, tmp);
		return true;
	}


	T front() const{
		return T(*begin());
	}

	void push_front(const T& val){
		Node<T>* newNode = Memory::CreateNew<Node<T>>(m_pool);
		newNode->object = val;
		newNode->lock.LockWrite();

		RGE_LogDebug(m_pushLogHandle, "push_front: Trying to get write lock");
		while(!m_front.load()->lock.TryLockWrite())
			std::this_thread::sleep_for(GenRandomTime(m_maxSleepTime));
		RGE_LogDebug(m_pushLogHandle, "push_front: m_front (%p) locked for writing", m_front.load());

		newNode->next = m_front;
		m_front.load()->prev = newNode;
		m_front = newNode;

		if(empty())
			m_back = newNode;
		m_size++;

		RGE_LogDebug(m_pushLogHandle, "push_front: Value %u is pushed_front: size = %u m_front = %p(%i), m_front->next = %p(%i)",
			val, m_size.load(), m_front.load(), m_front.load()->lock.IsLockedWrite(),
			m_front.load()->next, m_front.load()->next->lock.IsLockedWrite());

		m_front.load()->next->lock.UnlockWrite();
		m_front.load()->lock.UnlockWrite();

		RGE_LogDebug(m_pushLogHandle, "push_front: nodes unlocked: %p -> %p", m_front.load(), m_front.load()->next);
	}

	void pop_front(){
		while(!TryLockNodes(m_front, m_front.load()->next)){
			if(empty())	return;
			std::this_thread::sleep_for(GenRandomTime(m_maxSleepTime));
		}
		m_size--;

		Node<T>* tmp = m_front;
		m_front = m_front.load()->next;
		m_front.load()->prev = nullptr;

		if(m_front == m_end)
			m_back = m_end;

		m_front.load()->lock.UnlockWrite();
		Memory::Delete(m_pool, tmp);
	}

	int LockedNodesCount(){
		int count = 0;
		Node<T>* curNode = m_front;
		while(curNode != m_end){
			if(curNode->lock.IsLockedWrite())
				count++;
			curNode = curNode->next;
		}
		return count;
	}

	bool pop_front(T* val){
		RGE_LogDebug(m_popLogHandle, "pop_front: Trying to get write locks");

		Node<T>* tmp;				//DEBUG
		int repeats = 0;
		while(!TryLockNodes(m_front, m_front.load()->next)){
			tmp = m_front;
			if(empty())	return false;
			std::this_thread::sleep_for(GenRandomTime(m_maxSleepTime));

			repeats++;
		}
		m_size--;

		RGE_LogDebug(m_popLogHandle, "pop_front: Lock had been gotten for %p(%i) -> %p(%i) with %i reps and locked nodes count is %i of %u",
			m_front.load(), m_front.load()->lock.IsLockedWrite(),
			m_front.load()->next, m_front.load()->next->lock.IsLockedWrite(),
			repeats, LockedNodesCount(), m_size.load());

		*val = m_front.load()->object;
		tmp = m_front;
		m_front = m_front.load()->next;
		m_front.load()->prev = tmp->prev;

		if(m_front == m_end)
			m_back = m_end;

		try{
			m_front.load()->lock.UnlockWrite();
			Memory::Delete(m_pool, tmp);
			if(!empty()){
				RGE_LogDebug(m_popLogHandle, "pop_front: Value %u is popped: size = %u, m_front = %p(%i), m_front->next = %p(%i)",
					*val, m_size.load(), m_front.load(), m_front.load()->lock.IsLockedWrite(),
					m_front.load()->next, m_front.load()->next->lock.IsLockedWrite());
			}
			else{
				RGE_LogDebug(m_popLogHandle, "pop_front: Value %u is popped: size = %u, m_front = %p(%i), m_front->next = nullptr",
					*val, m_size.load(), m_front.load(), m_front.load()->lock.IsLockedWrite());
			}
		}
		catch(Exception::BaseException& e){
			tmp = m_front;
			int cnt = LockedNodesCount();

			RGE_LogDebug(m_popLogHandle, "pop_front: %s: m_front = %p, locked nodes count is %i of %u",
				e.What(), m_front.load(), cnt, m_size.load());

			throw e;
		}

		return true;
	}


	size_t size() const{
		return m_size;
	}

	size_t max_size() const{
		return -1;
	}

	bool empty() const{
		return m_size == 0;
	}

private:
	// вставляем элемент в список перед pos 
	/*	void Insert(ListIterator& pos, const T& val){

	}

	// удаляем элемент pos из списка
	void Delete(ListIterator& pos){

	}*/

	// Попытка блокировать 2 или 3 соседних узла на запись. Блокировка берется на всех узлах, либо не берется вообще
	bool TryLockNodes(Node<T>* node1, Node<T>* node2, Node<T>* node3 = nullptr) const{
		if(node1 == nullptr || node2 == nullptr)	return false;

		if(node1->lock.TryLockWrite()){
			if(node2->lock.TryLockWrite()){
				if(node3 == nullptr || node3->lock.TryLockWrite())
					return true;										// все нужные блокировки взяты

				node2->lock.UnlockWrite();								// не удалось залочить третий узел
			}
			node1->lock.UnlockWrite();									// не удалось залочить второй узел
		}

		return false;
	}

	//TODO: перенести куда-нибудь в область работы со временем, здесь этому не место
	std::chrono::milliseconds GenRandomTime(uint32_t maxSleepTime) const{
		std::uniform_int_distribution<uint32_t>		randomTime(1, maxSleepTime);
		std::default_random_engine					engine;

		return std::chrono::milliseconds(randomTime(engine));
	}

private:
	static Memory::PoolAllocator<Node<T>>	m_pool;

	//DEBUG
	Logging::LogHandle			m_pushLogHandle;
	Logging::LogHandle			m_popLogHandle;

	Spinlock					m_globalLock;
	std::atomic<Node<T>*>		m_front;
	std::atomic<Node<T>*>		m_back;
	Node<T>*					m_end;						// фиктивный узел для обозначения конца списка
	std::atomic<size_t>			m_size;
	uint32_t					m_maxSleepTime;
};

template <class T>
Memory::PoolAllocator< typename List<T>::Node<T> >	List<T>::m_pool(100, 4);

}
}