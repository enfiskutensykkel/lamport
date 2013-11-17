#ifndef __LOCKING_QUEUE_H__
#define __LOCKING_QUEUE_H__

#include "queue.h"
#include <string>
#include <cstdint>
#include <pthread.h>


/*
 * A contention queue that uses locks.
 */
class LockingQueue : public Queue
{
	public:
		explicit LockingQueue(uint32_t slots);

		virtual ~LockingQueue();

		virtual std::string type()
		{
			return "LockingQueue";
		};

		virtual bool enqueue(int element);

		virtual bool dequeue(int& reference);

		virtual uint32_t size();

	private:
		pthread_mutex_t lock;	// synchronization primitive
};

#endif
