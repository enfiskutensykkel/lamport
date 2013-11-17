#ifndef __ONE_TO_ONE_PRODUCER_H__
#define __ONE_TO_ONE_PRODUCER_H__

#include "producer.h"
#include "queue.h"
#include <string>
#include <cstdint>
#include <pthread.h>


class OneToOneProducer : public Producer
{
	public:
		OneToOneProducer(pthread_barrier_t& barrier, unsigned id, Queue<int>& queue, uint64_t reps)
			: Producer(barrier, id, queue, reps)
		{
		};

		virtual void run(Queue<int>& queue, const bool& running);

		virtual std::string getType()
		{
			return "OneToOne";
		};
};

#endif
