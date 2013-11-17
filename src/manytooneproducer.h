#ifndef __MANY_TO_ONE_PRODUCER_H__
#define __MANY_TO_ONE_PRODUCER_H__

#include "producer.h"
#include "queue.h"
#include <pthread.h>


class ManyToOneProducer : public Producer
{
	public:
		ManyToOneProducer(pthread_barrier_t& barrier, unsigned id, Queue<int>& queue)
			: Producer(barrier, id, queue)
		{
		};

		virtual void run(Queue<int>& queue, const bool& running);
};

#endif
