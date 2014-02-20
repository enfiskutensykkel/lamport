#ifndef __OPTIMIZED_LOCKING_QUEUE_H__
#define __OPTIMIZED_LOCKING_QUEUE_H__

#include "queue.h"
#include <string>
#include <cstdint>
#include <pthread.h>

/*
 * A contention queue that uses locks, but is slightly optimized.
 * We only have one consumer, which means that we only need to lock inserts.
 */
class OptimizedLockingQueue : public Queue
{
	public:
		explicit OptimizedLockingQueue(uint32_t slots);

		virtual ~OptimizedLockingQueue();

		virtual std::string type()
		{
			return "OptimizedLockingQueue";
		};

		virtual bool enqueue(int element);

		virtual bool dequeue(int& reference);

		virtual uint32_t size();

	private:
		pthread_mutex_t tail_lock;	// synchronization primitive
		const uint32_t mask;     	// calculate index in buffer without doing mod/div
};

#endif
