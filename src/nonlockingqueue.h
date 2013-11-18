#ifndef __NON_LOCKING_QUEUE_H__
#define __NON_LOCKING_QUEUE_H__

#include "queue.h"
#include <cstdint>
#include <string>


/*
 * An implementation of a non-locking queue.
 */
class NonLockingQueue : public Queue
{
	public:
		explicit NonLockingQueue(uint32_t slots);

		virtual ~NonLockingQueue();

		virtual std::string type()
		{
			return "NonLockingQueue";
		};

		virtual bool enqueue(int element);

		virtual bool dequeue(int& reference);

		virtual uint32_t size();

	private:
		const uint32_t mask;
};

#endif
