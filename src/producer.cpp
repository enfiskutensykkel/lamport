#include "producer.h"
#include <pthread.h>
#include <stdexcept>
#include <cstdint>
#include <assert.h>



/* Spawn a new producer thread */
Producer::Producer(pthread_barrier_t& b, unsigned id, Queue& q, unsigned r)
	: id(id), repetitions(r), barrier(b), queue(q), running(true)
{
	if (pthread_create(&thread, nullptr, (void* (*)(void*)) &Producer::dispatch, static_cast<void*>(this)) != 0)
	{
		assert(false);
		throw std::runtime_error("failed to create thread");
	}
}



/* Destroy the producer and join with thread */
Producer::~Producer()
{
	running = false;
	if (pthread_join(thread, nullptr) != 0)
	{
		assert(false);
	}
}



/* Helper method to dispatch and synchronize threads */
void Producer::dispatch(Producer* thread)
{
	int ret = pthread_barrier_wait(&thread->barrier);
	if (ret != PTHREAD_BARRIER_SERIAL_THREAD && ret != 0)
	{
		assert(false);
		throw std::runtime_error("failed to wait on barrier in thread");
	}

	if (thread->running)
	{
		thread->run(thread->queue, thread->running);
	}
}



/* Get the difference between two timespecs */
uint64_t Producer::tsdiff(const timespec& minuend, const timespec& subtrahend)
{
	timespec difference;

	if ((minuend.tv_sec < subtrahend.tv_sec) || 
			((minuend.tv_sec == subtrahend.tv_sec) && (minuend.tv_nsec <= subtrahend.tv_nsec)))
	{
		difference.tv_sec = difference.tv_nsec = 0;
	}
	else
	{
		difference.tv_sec = minuend.tv_sec - subtrahend.tv_sec;
		if (minuend.tv_nsec < subtrahend.tv_nsec)
		{
			difference.tv_nsec = minuend.tv_nsec + 1000000000L - subtrahend.tv_nsec;
			--difference.tv_sec;
		}
		else
		{
			difference.tv_nsec = minuend.tv_nsec - subtrahend.tv_nsec;
		}
	}

	return difference.tv_sec * 1000000000L + difference.tv_nsec;
}
