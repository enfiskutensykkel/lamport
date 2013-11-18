#include "queue.h"
#include "lockingqueue.h"
#include <stdexcept>
#include <cstdint>
#include <pthread.h>
#include <assert.h>



/* Create a locking queue and initialize synchronization primitives */
LockingQueue::LockingQueue(uint32_t slots)
	: Queue(slots)
{
	if (pthread_mutex_init(&lock, nullptr) != 0)
	{
		assert(false);
		throw std::runtime_error("failed to initialize queue lock");
	}
}



/* Free resources */
LockingQueue::~LockingQueue()
{
	if (pthread_mutex_destroy(&lock) != 0)
	{
		assert(false);
	}
}



/* Enqueue an element */
bool LockingQueue::enqueue(int element)
{
	if (pthread_mutex_lock(&lock) != 0)
	{
		assert(false);
		throw std::runtime_error("failed to take lock in enqueue");
	}

	if (tail - head >= capacity)
	{
		pthread_mutex_unlock(&lock);
		return false;
	}

	buffer[tail % capacity] = new int(element);
	++tail;

	pthread_mutex_unlock(&lock);
	return true;
}



/* Dequeue an element */
bool LockingQueue::dequeue(int& element)
{
	if (pthread_mutex_lock(&lock) != 0)
	{
		assert(false);
		throw std::runtime_error("failed to take lock in dequeue");
	}

	if (tail == head)
	{
		pthread_mutex_unlock(&lock);
		return false;
	}

	element = *buffer[head % capacity];
	delete buffer[head % capacity];
	buffer[head % capacity] = nullptr;
	++head;

	pthread_mutex_unlock(&lock);
	return true;
}



/* Return the number of elements currently enqueued */
uint32_t LockingQueue::size()
{
	if (pthread_mutex_lock(&lock) != 0)
	{
		assert(false);
		throw std::runtime_error("failed to take lock in size");
	}

	uint32_t size = tail - head;

	pthread_mutex_unlock(&lock);
	return size;
}
