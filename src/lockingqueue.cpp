#include "queue.h"
#include "lockingqueue.h"
#include <pthread.h>
#include <stdexcept>



template <>
LockingQueue<int>::LockingQueue(unsigned slots)
	: Queue(slots)
{
	if (pthread_mutex_init(&lock, nullptr) != 0)
	{
		throw std::runtime_error("failed to initialize queue lock");
	}
}



template <>
LockingQueue<int>::~LockingQueue()
{
	if (pthread_mutex_destroy(&lock) != 0)
	{
	}
}



template <>
bool LockingQueue<int>::enqueue(const int& element)
{
	if (pthread_mutex_lock(&lock) != 0)
	{
		throw std::runtime_error("failed to take lock in enqueue");
	}

	if (tail - head >= capacity)
	{
		pthread_mutex_unlock(&lock);
		return false;
	}

	queue[tail % capacity] = new int(element);
	++tail;

	pthread_mutex_unlock(&lock);
	return true;
}



template <>
bool LockingQueue<int>::dequeue(int& element)
{
	if (pthread_mutex_lock(&lock) != 0)
	{
		throw std::runtime_error("failed to take lock in dequeue");
	}

	if (tail == head)
	{
		pthread_mutex_unlock(&lock);
		return false;
	}

	element = *queue[head % capacity];
	delete queue[head % capacity];
	++head;

	pthread_mutex_unlock(&lock);
	return true;
}



template <>
unsigned LockingQueue<int>::size()
{
	if (pthread_mutex_lock(&lock) != 0)
	{
		throw std::runtime_error("failed to take lock in size");
	}

	unsigned size = tail - head;

	pthread_mutex_unlock(&lock);
	return size;
}



template <>
bool LockingQueue<int>::empty()
{
	return size() == 0;
}
