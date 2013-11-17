#ifndef __MANY_TO_ONE_PRODUCER_H__
#define __MANY_TO_ONE_PRODUCER_H__

#include "producer.h"
#include "queue.h"
#include <string>
#include <cstdint>
#include <pthread.h>


class ManyToOneProducer : public Producer
{
	public:
		ManyToOneProducer(pthread_barrier_t& barrier, unsigned id, Queue<int>& queue, uint64_t reps)
			: Producer(barrier, id, queue, reps)
		{
		};

		virtual void run(Queue<int>& queue, const bool& running);

		virtual std::string getType()
		{
			return "ManyToOne";
		};
};

#endif
