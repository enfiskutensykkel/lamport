#include "producer.h"
#include "manytooneproducer.h"
#include "onetooneproducer.h"
#include "lockingqueue.h"
#include "lamportqueue.h"
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



uint64_t validate(Queue<int>& queue, std::vector<producer<ManyToOneProducer> >& producers, unsigned repetitions)
{
	timespec start, stop;
	uint64_t i = repetitions * producers.size() + 1;
	uint64_t* count = new uint64_t[producers.size()];
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

	for (std::vector<producer<ManyToOneProducer> >::iterator i = producers.begin(); i != producers.end(); i++)
	{
		if (count[i->implementation->getId()] != repetitions)
		{
			i->failed = true;
		}
	}

	delete[] count;
	return Producer::diff(stop, start);
}



uint64_t validate(Queue<int>& queue, std::vector<producer<OneToOneProducer> >& producers, unsigned repetitions)
{
	timespec start, stop;
	uint64_t i = repetitions * producers.size() + 1;

	bool got_unexpected = false;
	clock_gettime(CLOCK_REALTIME, &start);
	while (--i != 0)
	{
		int element;
		while (!queue.dequeue(element))
		{
			pthread_yield();
		}

		if ((uint64_t) element != i)
		{
			got_unexpected = true;
		}
	}
	clock_gettime(CLOCK_REALTIME, &stop);

	for (std::vector<producer<OneToOneProducer> >::iterator i = producers.begin(); i != producers.end(); i++)
	{
		if (got_unexpected)
		{
			i->failed = true;
		}
	}

	return Producer::diff(stop, start);
}




/* Run a validation test on a queue specialization */
template <class Producer>
bool run_test(Queue<int>& queue, unsigned repetitions, unsigned num_producers)
{
	std::vector<producer<Producer> > producers;

	pthread_barrier_t barrier;
	if (pthread_barrier_init(&barrier, nullptr, num_producers + 1) != 0)
	{
		return false;
	}

	fprintf(stderr, "%-40s", "Creating producers");
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

	fprintf(stderr, "%-40s", "Synchronizing threads");
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

	fprintf(stderr, "%-40s", "Running test");
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
	fprintf(stdout, "Queue type    : %s\n", queue.getName().c_str());
	fprintf(stdout, "Queue size    : %u elements left (%s)\n", queue.size(), queue.empty() && queue.size() == 0 ? "\033[0;92mPASS\033[0m" : "\033[0;91mFAIL\033[0m");
	fprintf(stdout, "Producer type : %s\n", producers[0].implementation->getType().c_str());
	fprintf(stdout, "#Producers    : %u threads\n", num_producers);
	fprintf(stdout, "Total duration: %.5f seconds\n", duration / (1000000000.0));
	for (unsigned i = 0; i < num_producers; ++i)
	{
		fprintf(stdout, "    %2u\t%s\n", i, 
				producers[i].failed ? "\033[0;91mFAIL\033[0m" : "\033[0;92mPASS\033[0m" );
		failed += producers[i].failed;
	}
	fprintf(stdout, "%u out of %u producer threads passed\n", (num_producers - failed), num_producers);
	fprintf(stdout, "\n");

	return failed == 0;
}



int main(void)
{
	int fail_count = 0;
	LockingQueue<int> queue(QUEUE_SIZE);

	fail_count += !run_test<OneToOneProducer>(queue, REPETITIONS, 1);
	
	fail_count += !run_test<ManyToOneProducer>(queue, REPETITIONS, PRODUCERS);

	return fail_count == 0;
}
