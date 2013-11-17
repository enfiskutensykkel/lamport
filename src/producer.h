#ifndef __PRODUCER_H__
#define __PRODUCER_H__

#include "queue.h"
#include <string>
#include <cstdint>
#include <pthread.h>


/*
 * An abstract class defining how a producer thread should work.
 */
class Producer
{
	public:
		/* Create a producer thread that enqueues a number of elements into a queue */
		Producer(pthread_barrier_t& barrier, unsigned id, Queue& queue, unsigned elements);

		/* Destroy the producer and join threads */
		virtual ~Producer();

		/* 
		 * Produce elements and store them in queue.
		 * This abstract method should be implemented by deriving classes.
		 */
		virtual void run(Queue& queue, const bool& running) = 0;

		/* Return a short name describing the producer type */
		virtual std::string type() = 0;

		/* Calculate the difference between two timestamps and return the result in nanoseconds */
		static uint64_t tsdiff(const timespec&, const timespec&);

		/* The producer thread ID */
		const unsigned id;

		/* The number of elements the producer will create */
		const unsigned repetitions;

	private:
		pthread_barrier_t& barrier;	// synchronization primitive
		Queue& queue;             	// the queue to push elements to
		bool running;              	// should we continue running?
		pthread_t thread;         	// actual thread PID

		/* Helper method to dispatch a producer thread */
		static void dispatch(Producer* thread);
};

#endif
