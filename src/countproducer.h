#ifndef __ONE_TO_ONE_PRODUCER_H__
#define __ONE_TO_ONE_PRODUCER_H__

#include "producer.h"
#include "queue.h"
#include <string>
#include <pthread.h>


/*
 * A producer implementation that produces a number range.
 */
class CountProducer : public Producer
{
	public:
		CountProducer(pthread_barrier_t& barrier, unsigned id, Queue& queue, unsigned limit)
			: Producer(barrier, id, queue, limit)
		{
		};

		virtual void run(Queue& queue, const bool& running);

		virtual std::string type()
		{
			return "CountProducer";
		};
};

#endif
