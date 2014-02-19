#include "producer.h"
#include "idrepeater.h"
#include "countproducer.h"
#include "locking.h"
#include "optimized.h"
#include "lamport.h"
#include <vector>
#include <memory>
#include <stdexcept>
#include <cstdio>
#include <cstdint>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>


template <class Producer>
struct producer
{
	bool failed;
	std::shared_ptr<Producer> implementation;

	producer(Producer* p)
		: failed(false), implementation( std::shared_ptr<Producer>(p) )
	{
	};
};



uint64_t validate(Queue& queue, std::vector<producer<IdRepeater> >& producers, unsigned repetitions)
{
	timespec start, stop;
	unsigned i = repetitions * producers.size() + 1;
	unsigned* count = new unsigned[producers.size()];
	for (unsigned k = 0; k < producers.size(); ++k)
	{
		count[k] = 0;
	}

	clock_gettime(CLOCK_REALTIME, &start);
	while (--i != 0)
	{
		int id;
		while (!queue.dequeue(id))
		{
			pthread_yield();
		}

		++count[id];
	}
	clock_gettime(CLOCK_REALTIME, &stop);

	for (producer<IdRepeater>& p: producers)
	{
		if (count[p.implementation->id] != repetitions)
		{
			p.failed = true;
		}
	}

	delete[] count;
	return Producer::tsdiff(stop, start);
}



uint64_t validate(Queue& queue, std::vector<producer<CountProducer> >& producers, unsigned repetitions)
{
	timespec start, stop;
	unsigned i;

	unsigned* counts = new unsigned[repetitions];
	for (i = 1; i < repetitions + 1; ++i)
	{
		counts[i] = 0;
	}

	i = repetitions * producers.size() + 1;
	clock_gettime(CLOCK_REALTIME, &start);
	while (--i != 0)
	{
		int element;
		while (!queue.dequeue(element))
		{
			pthread_yield();
		}

		++counts[element];

	}
	clock_gettime(CLOCK_REALTIME, &stop);

	for (i = 1; i < repetitions + 1; ++i)
	{
		if (counts[i] != producers.size())
		{
			printf("counts[%u] = %u\n", i, counts[i]);
			break;
		}
	}

	delete[] counts;
	for (producer<CountProducer>& p: producers)
	{
		if (i != repetitions + 1)
		{
			p.failed = true;
		}
	}

	return Producer::tsdiff(stop, start);
}




/* Run a validation test on a queue specialization */
template <class Producer>
bool run_test(Queue& queue, unsigned repetitions, unsigned num_producers)
{
	std::vector<producer<Producer> > producers;

	pthread_barrier_t barrier;
	if (pthread_barrier_init(&barrier, nullptr, num_producers + 1) != 0)
	{
		return false;
	}

	fprintf(stderr, "Testing %s...\n", queue.type().c_str());
	fprintf(stderr, "%-25s", "Creating producers");
	for (unsigned i = 0; i < num_producers; ++i)
	{
		try
		{
			producers.push_back( new Producer(barrier, i, queue, repetitions) );
		}
		catch (const std::runtime_error& e)
		{
			fprintf(stderr, "FAIL\n");
			return false;
		}
	}
	fprintf(stderr, "DONE\n");

	fprintf(stderr, "%-25s", "Synchronizing threads");
	int ret = pthread_barrier_wait(&barrier);
	if (ret != PTHREAD_BARRIER_SERIAL_THREAD && ret != 0)
	{
		producers.clear();
		pthread_barrier_destroy(&barrier);
		fprintf(stderr, "FAIL\n");
		return false;
	}	
	fprintf(stderr, "DONE\n");


	unsigned failed;
	uint64_t duration;

	fprintf(stderr, "%-25s", "Running test");
	try
	{
		failed = 0;
		duration = validate(queue, producers, repetitions);
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

	fprintf(stdout, "\033[1;96mTest result\033[0m\n");
	fprintf(stdout, "Queue type    : %s\n", queue.type().c_str());
	fprintf(stdout, "Queue capacity: %u slots\n", queue.capacity);
	fprintf(stdout, "Queue size    : %u elements left (%s)\n", queue.size(), queue.empty() && queue.size() == 0 ? "\033[0;92mPASS\033[0m" : "\033[0;91mFAIL\033[0m");
	fprintf(stdout, "Queue elements: %u elements per thread\n", repetitions);
	fprintf(stdout, "Producer type : %s\n", producers[0].implementation->type().c_str());
	fprintf(stdout, "Producers     : %u threads\n", num_producers);
	fprintf(stdout, "Total duration: %.5f seconds\n", duration / (1000000000.0));
	for (unsigned i = 0; i < num_producers; ++i)
	{
		fprintf(stdout, "    %2u\t%s\n", i + 1, 
				producers[i].failed ? "\033[0;91mFAIL\033[0m" : "\033[0;92mPASS\033[0m" );
		failed += producers[i].failed;
	}
	fprintf(stdout, "%u out of %u producer threads passed\n", (num_producers - failed), num_producers);
	fprintf(stdout, "\n");

	return failed == 0;
}



int main(void)
{
	LockingQueue locking(QUEUE_SIZE);
	OptimizedLockingQueue locking_optimized(QUEUE_SIZE);
	LamportQueue lockfree(QUEUE_SIZE);

	int fail_count = 0;

	fail_count += !run_test<CountProducer>(locking, REPETITIONS, PRODUCERS);
	fail_count += !run_test<CountProducer>(locking_optimized, REPETITIONS, PRODUCERS);
	fail_count += !run_test<CountProducer>(lockfree, REPETITIONS, PRODUCERS);
	
	fail_count += !run_test<IdRepeater>(locking, REPETITIONS, PRODUCERS);
	fail_count += !run_test<IdRepeater>(locking_optimized, REPETITIONS, PRODUCERS);
	fail_count += !run_test<IdRepeater>(lockfree, REPETITIONS, PRODUCERS);

	return fail_count != 0;
}
