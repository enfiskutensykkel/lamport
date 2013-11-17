#ifndef __ONE_TO_ONE_PRODUCER_H__
#define __ONE_TO_ONE_PRODUCER_H__

#include "producer.h"
#include "queue.h"
#include <pthread.h>


class OneToOneProducer : public Producer
{
	public:
		OneToOneProducer(pthread_barrier_t& barrier, unsigned id, Queue<int>& queue)
			: Producer(barrier, id, queue)
		{
		};

		virtual void run(Queue<int>& queue, const bool& running);
};

#endif
