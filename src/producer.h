#ifndef __PRODUCER_H__
#define __PRODUCER_H__

#include "queue.h"
#include <string>
#include <cstdint>
#include <pthread.h>


class Producer
{
	public:
		Producer(pthread_barrier_t& barrier, unsigned id, Queue<int>& queue, uint64_t repetitions);

		virtual ~Producer();

		virtual void run(Queue<int>& queue, const bool& running) = 0;

		virtual std::string getType() = 0;

		unsigned getId();

		uint64_t getRepetitions();

		static uint64_t diff(const timespec&, const timespec&);

	private:
		pthread_barrier_t& barrier;
		Queue<int>& queue;
		unsigned id;
		bool running;
		pthread_t thread;
		uint64_t reps;

		static void dispatch(Producer* thread);
};

#endif
