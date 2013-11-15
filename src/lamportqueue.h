#ifndef __LAMPORT_QUEUE_H__
#define __LAMPORT_QUEUE_H__

#include "queue.h"


template <typename T = int>
class LamportQueue : public Queue<T>
{
	public:
		explicit LamportQueue(unsigned slots);

		virtual ~LamportQueue();

		virtual bool enqueue(const T& element);

		virtual bool dequeue(T& element);

		virtual unsigned size();

		virtual bool empty();
};

#endif
