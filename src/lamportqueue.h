#ifndef __LAMPORT_QUEUE_H__
#define __LAMPORT_QUEUE_H__

#include "queue.h"
#include <cstdint>
#include <string>


/*
 * An implementation of a Lamport's queue/
 * This only works with one producer and one or more consumers.
 */
class LamportQueue : public Queue
{
	public:
		explicit LamportQueue(uint32_t slots);

		virtual ~LamportQueue();

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
