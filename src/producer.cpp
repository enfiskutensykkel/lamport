#include "producer.h"
#include <pthread.h>
#include <vector>
#include <memory>
#include <utility>
#include <stdexcept>
#include <cstdint>
#include <cstdio>
#include <assert.h>
#include <sys/time.h>
#include <string.h>

typedef std::shared_ptr<Producer> ProducerPtr;
typedef std::pair<ProducerPtr, bool> TProducer;


/* 
 * Helper function to get the difference between two timespecs
 * Calculate the difference between two timestamps and return the result in nanoseconds 
 */
static inline uint64_t tsdiff(const timespec& minuend, const timespec& subtrahend)
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



/* Helper function to validate a queue */
static uint64_t test_dequeuing(Queue& queue, std::vector<TProducer>& producers, unsigned repetitions)
{
	timespec start, stop;
	unsigned* count = new unsigned[producers.size()];
	bzero((void*) count, producers.size()); // not very C++-ish, C++ purists would complain about this

#ifndef NDEBUG
	for (TProducer& test: producers)
	{
		assert(test.first->value >= 0 && test.first->value < producers.size());
	}
#endif

	unsigned n = repetitions * producers.size() + 1;
	clock_gettime(CLOCK_REALTIME, &start);
	while (--n != 0)
	{
		// Try to dequeue an element from the queue
		// and yield if the queue is (temporarily) empty
		int value;
		while (!queue.dequeue(value))
		{
			pthread_yield();
		}

		assert(value >= 0 && value < producers.size());
		++count[value];
	}
	clock_gettime(CLOCK_REALTIME, &stop);

	for (TProducer& test: producers)
	{
		test.second = count[test.first->value] == repetitions;
	}

	delete[] count;
	return tsdiff(stop, start);
}



/* Run a validation test on a queue */
bool Producer::test_queue(Queue& queue, unsigned repetitions, unsigned num_threads)
{
	std::vector<TProducer> tests;

	fprintf(stderr, "Testing %s...\n", queue.type().c_str());

	// Create a barrier to synchronize threads
	pthread_barrier_t barrier;
	if (pthread_barrier_init(&barrier, nullptr, num_threads + 1) != 0)
	{
		assert(false);
		return false;
	}

	// Create and start threads
	fprintf(stderr, "%-25s", "Creating producers");
	for (unsigned i = 0; i < num_threads; ++i)
	{
		try
		{
			tests.push_back(std::make_pair(std::shared_ptr<Producer>( new Producer(barrier, i, queue, repetitions) ), true));
		}
		catch (const std::runtime_error& e)
		{
			fprintf(stderr, "FAIL\n");
			return false;
		}
	}
	fprintf(stderr, "DONE\n");

	// Synchronize with threads
	fprintf(stderr, "%-25s", "Synchronizing threads");
	int ret = pthread_barrier_wait(&barrier);
	if (ret != PTHREAD_BARRIER_SERIAL_THREAD && ret != 0)
	{
		tests.clear(); // this invokes the destructor on all producers
		pthread_barrier_destroy(&barrier);
		fprintf(stderr, "FAIL\n");
		return false;
	}
	fprintf(stderr, "DONE\n");

	// Run tests
	uint64_t duration;
	fprintf(stderr, "%-25s", "Running test");
	try
	{
		duration = test_dequeuing(queue, tests, repetitions);
		fprintf(stderr, "%s\n", "DONE");
	}
	catch (const std::runtime_error& e)
	{
		pthread_barrier_destroy(&barrier);
		fprintf(stderr, "%s\n", "FAIL");
		return false;
	}
	pthread_barrier_destroy(&barrier);
	fprintf(stderr, "\n");

	// Write summary
	fprintf(stdout, "\033[1;96mTest result\033[0m\n");
	fprintf(stdout, "Queue type    : %s\n", queue.type().c_str());
	fprintf(stdout, "Queue capacity: %u slots\n", queue.capacity);
	fprintf(stdout, "Queue size    : %u elements left (%s)\n", queue.size(), queue.empty() && queue.size() == 0 ? "\033[0;92mPASS\033[0m" : "\033[0;91mFAIL\033[0m");
	fprintf(stdout, "Queue elements: %u elements per thread\n", repetitions);
	fprintf(stdout, "Producers     : %u threads\n", num_threads);
	fprintf(stdout, "Total duration: %.5f seconds\n", duration / (1000000000.0));

	unsigned failed = 0;
	for (unsigned i = 0; i < tests.size(); ++i)
	{
		fprintf(stdout, "    %2u\t%s\n", i + 1, tests[i].second ? "\033[0;92mPASS\033[0m" : "\033[0;91mFAIL\033[0m" ); 
		failed += !tests[i].second;
	}
	fprintf(stdout, "%u out of %u producer threads passed\n", (num_threads - failed), num_threads);
	fprintf(stdout, "\n");

	return failed == 0;
}



/* Start a new producer thread */
Producer::Producer(pthread_barrier_t& b, int value, Queue& q, unsigned repetitions)
	: value(value), repetitions(repetitions), barrier(b), queue(q), running(true)
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

	assert(ret == PTHREAD_BARRIER_SERIAL_THREAD || ret == 0);
	if (ret != PTHREAD_BARRIER_SERIAL_THREAD && ret != 0)
	{
		throw std::runtime_error("failed to wait on barrier in thread");
	}

	if (thread->running)
	{
		thread->run();
	}
}



/* Repeat the given ID #repetition times */
void Producer::run()
{
	unsigned remaining = repetitions + 1;

	while (running && --remaining != 0)
	{
		// Try to insert element into queue
		// If it's full, yield and try again later
		while (running && !queue.enqueue(value))
		{
			pthread_yield();
		}
	}
}

