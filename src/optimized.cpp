#include "queue.h"
#include "optimized.h"
#include <stdexcept>
#include <cstdint>
#include <pthread.h>
#include <assert.h>



/* Create a locking queue and initialize synchronization primitives */
OptimizedLockingQueue::OptimizedLockingQueue(uint32_t slots)
	: Queue(slots), mask(capacity - 1)
{
	if (pthread_mutex_init(&tail_lock, nullptr) != 0)
	{
		assert(false);
		throw std::runtime_error("failed to initialize queue lock");
	}
}



/* Free resources */
OptimizedLockingQueue::~OptimizedLockingQueue()
{
	if (pthread_mutex_destroy(&tail_lock) != 0)
	{
		assert(false);
	}
}



/* Enqueue an element */
bool OptimizedLockingQueue::enqueue(int element)
{
	if (pthread_mutex_lock(&tail_lock) != 0)
	{
		assert(false);
		throw std::runtime_error("failed to take lock in enqueue");
	}

	if (tail - head >= capacity)
	{
		pthread_mutex_unlock(&tail_lock);
		return false;
	}

	uint64_t curr_tail = tail++;
	pthread_mutex_unlock(&tail_lock);

	buffer[curr_tail & mask] = new int(element);

	return true;
}



/* Dequeue an element */
bool OptimizedLockingQueue::dequeue(int& element)
{
	if (tail == head)
	{
		return false;
	}

	int* ptr;
	do
	{
		ptr = buffer[head & mask];
	} 
	while (ptr == nullptr);

	element = *ptr;

	delete buffer[head & mask];
	buffer[head & mask] = nullptr;

	++head;

	return true;
}



/* Return the number of elements currently enqueued */
uint32_t OptimizedLockingQueue::size()
{
	return tail - head;
}
