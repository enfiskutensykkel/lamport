#ifndef __NON_LOCKING_QUEUE_H__
#define __NON_LOCKING_QUEUE_H__

#include "queue.h"
#include <cstdint>
#include <string>


/*
 * An implementation of a non-locking (lock-free) queue.
 */
class LamportQueue : public Queue
{
	public:
		explicit LamportQueue(uint32_t slots);

		virtual ~LamportQueue();

		virtual std::string type()
		{
			return "LamportQueue";
		};

		virtual bool enqueue(int element);

		virtual bool dequeue(int& reference);

		virtual uint32_t size();

	private:
		// calculate index in buffer without doing expensive modulo operations
		const uint32_t mask;
};

#endif
