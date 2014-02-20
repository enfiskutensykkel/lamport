#ifndef __PRODUCER_H__
#define __PRODUCER_H__

#include "queue.h"
#include <string>
#include <cstdint>
#include <pthread.h>


/*
 * A wrapper class for creating a producer thread that tries to insert a value 
 * into the queue N times.
 */
class Producer
{
	public:
		/* Destroy the producer and join threads */
		~Producer();

		/* Produce elements and store them in the queue */
		void run();

		/* The value to produce elements of */
		const int value;

		/* The number of elements the producer will create */
		const unsigned repetitions;

		/* Run a validation test on a queue, using a number of producer threads */
		static bool test_queue(Queue& queue, unsigned repetitions, unsigned producer_threads);

	private:
		pthread_barrier_t& barrier;	// synchronization primitive
		Queue& queue;             	// the queue to push elements to
		bool running;              	// should we continue running?
		pthread_t thread;         	// actual thread PID

		/* Create a producer thread that enqueues a number of elements into a queue */
		Producer(pthread_barrier_t& barrier, int value, Queue& queue, unsigned elements);

		/* Helper method to dispatch a producer thread */
		static void dispatch(Producer* thread);
};

#endif
