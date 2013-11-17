#ifndef __MANY_TO_ONE_PRODUCER_H__
#define __MANY_TO_ONE_PRODUCER_H__

#include "producer.h"
#include "queue.h"
#include <string>
#include <pthread.h>


/*
 * A producer implementation that repeats its ID.
 */
class IdRepeater : public Producer
{
	public:
		IdRepeater(pthread_barrier_t& barrier, unsigned id, Queue& queue, unsigned repetitions)
			: Producer(barrier, id, queue, repetitions)
		{
		};

		virtual void run(Queue& queue, const bool& running);

		virtual std::string type()
		{
			return "IdRepeater";
		};
};

#endif
