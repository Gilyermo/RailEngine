/********************************
*	Потокобезопасный контейнер;	*
********************************/
#pragma once

#include "Multithreading.h"
#include "RWSpinlock.h"
#include <shared_mutex>


namespace RGE
{
namespace Multithreading
{

template <class Container>
class SafeContainer{
public:
	using T				=	typename Container::value_type;
	using WriteLock		=	std::unique_lock<std::shared_timed_mutex>;
	using ReadLock		=	std::shared_lock<std::shared_timed_mutex>;

private:
	SafeContainer(SafeContainer&&) = delete;

public:
	SafeContainer(){}
	SafeContainer(const SafeContainer& cont){
		WriteLock lock(m_rwlock);
		WriteLock rhsLock(cont.m_rwlock);

		m_container = cont.m_container;
	}
	~SafeContainer(){}


	void resize(size_t n){
		WriteLock lock(m_rwlock);
		m_container.resize(n);
	}

	void resize(size_t n, const T& val){
		WriteLock lock(m_rwlock);
		m_container.resize(n, val);
	}


	T& front(){
		WriteLock lock(m_rwlock);
		return m_container.front();
	}

	const T& front() const{
		ReadLock lock(m_rwlock);
		return m_container.front();
	}
	
	void push_front(const T& val){
		WriteLock lock(m_rwlock);
		m_container.push_front(val);
	}

	void push_front(T&& val){
		WriteLock lock(m_rwlock);
		m_container.push_front(std::move(val));
	}

	void pop_front(){
		WriteLock lock(m_rwlock);
		m_container.pop_front();
	}

	bool try_get_and_pop_front(T* elem){
		WriteLock lock(m_rwlock);
		if(m_container.empty())
			return false;

		*elem = m_container.front();
		m_container.pop_front();
		return true;
	}


	T& back(){
		WriteLock lock(m_rwlock);
		return m_container.back();
	}

	const T& back() const{
		ReadLock lock(m_rwlock);
		return m_container.back();
	}

	void push_back(const T& val){
		WriteLock lock(m_rwlock);
		m_container.push_back(val);
	}

	void push_back(T&& val){
		WriteLock lock(m_rwlock);
		m_container.push_back(std::move(val));
	}

	void pop_back(){
		WriteLock lock(m_rwlock);
		m_container.pop_back();
	}

	bool try_get_and_pop_back(T* elem){
		WriteLock lock(m_rwlock);
		if(m_container.empty())
			return false;

		*elem = m_container.back();
		m_container.pop_back();
		return true;
	}


	bool try_erase(const T& elem){
		WriteLock lock(m_rwlock);
		if(m_container.empty())
			return false;

		auto itEnd = m_container.end();
		for(auto it=m_container.begin(); it!=itEnd; ++it){
			if(*it == elem){
				m_container.erase(it);
				return true;
			}
		}

		return false;
	}


// будет перебирать элементы, пока oper возвращает true
	template <class Operation>
	Operation for_each(Operation oper) const{
		WriteLock lock(m_rwlock);
		for(auto it = m_container.begin(); it != m_container.end() && oper(*it); ++it);
		return oper;
	}


	template <class Pred>
	void remove_first_if(Pred pred){
		WriteLock lock(m_rwlock);
		
		auto it = m_container.begin();
		while(it != m_container.end()){
			if(pred(*it))
				m_container.erase(it);
			++it;
		}
	}


	T& operator[](size_t indx){
		WriteLock lock(m_rwlock);
		return m_container[indx];
	}

	const T& operator[](size_t indx) const{
		ReadLock lock(m_rwlock);
		return m_container[indx];
	}


	size_t size() const{
		ReadLock lock(m_rwlock);
		return m_container.size();
	}

	bool empty() const{
		ReadLock lock(m_rwlock);
		return m_container.empty();
	}


// функция используется, чтобы заблокировать контейнер и получить затем его небезопасный экземпляр.
// Вызов других функций до разблокировки приведет к зависанию потока
	void lock() const{
		m_rwlock.lock();
	}
	void unlock() const{
		m_rwlock.unlock();
	}

	Container& get_unsafe(){
		return m_container;
	}

	const Container& get_unsafe() const{
		return m_container;
	}

private:
	Container							m_container;
	mutable std::shared_timed_mutex		m_rwlock;
};

}
}