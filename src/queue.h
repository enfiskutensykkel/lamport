#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <string>
#include <cstdint>


/*
 * An abstract class defining how a queue implementation should work.
 */
class Queue
{
	public:
		/* Allocate a queue buffer with at guaranteed number of slots */
		explicit Queue(uint32_t slots);

		/* Destroy the queue and free the queue buffer */
		virtual ~Queue();
		
		/* Return a short name describing the queue type */
		virtual std::string type() = 0;

		/*
		 * Enqueue an element.
		 * Return true if the element was placed in the queue successfully.
		 * Return false if the implementation failed to enqueue the element.
		 */
		virtual bool enqueue(int element) = 0;

		/*
		 * Dequeue an element.
		 * Return true if the element was dequeued and placed in the reference.
		 * Return false if no element was dequeued.
		 */
		virtual bool dequeue(int& reference) = 0;

		/* Return the size of the queue */
		virtual uint32_t size() = 0;

		/* Returns true if the queue has no elements enqueued, false otherwise */
		virtual bool empty();

		/* The queue capacity (size of the queue buffer) */
		const uint32_t capacity;

	protected:
		uint64_t head;	// index of the first element in the queue
		uint64_t tail;	// index of the last element in the queue
		int** buffer;	// the queue buffer
};

#endif
