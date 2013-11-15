#ifndef __PRODUCER_H__
#define __PRODUCER_H__

#include "queue.h"
#include <pthread.h>


class Producer
{
	public:
		Producer(pthread_barrier_t& barrier, unsigned id, Queue<int>& queue);

		~Producer();

		void run();

	private:
		pthread_barrier_t& barrier;
		Queue<int>& queue;
		unsigned id;
		bool running;
		pthread_t thread;

		static void dispatch(Producer* thread);
};

#endif
