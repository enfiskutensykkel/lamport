#ifndef __PRODUCER_H__
#define __PRODUCER_H__

#include "queue.h"
#include <pthread.h>
#include <cstdint>

class Producer
{
	public:
		Producer(pthread_barrier_t& barrier, unsigned id, Queue<int>& queue);

		virtual ~Producer();

		virtual void run(Queue<int>& queue, const bool& running) = 0;

		unsigned getId();

		static uint64_t diff(const timespec&, const timespec&);

	private:
		pthread_barrier_t& barrier;
		Queue<int>& queue;
		unsigned id;
		bool running;
		pthread_t thread;

		static void dispatch(Producer* thread);
};

#endif
