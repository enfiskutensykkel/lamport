#include "producer.h"
#include "manytooneproducer.h"
#include "lockingqueue.h"
#include "lamportqueue.h"
#include <vector>
#include <memory>
#include <stdexcept>
#include <cstdio>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>



typedef std::vector< std::shared_ptr<Producer> > producer_vec;
static producer_vec producers;



/* Exit quick and dirty */
static void signal_handler(int signal)
{
	switch (signal)
	{
		case SIGHUP:
		case SIGTERM:
		case SIGINT:
			// Remove all thread references and force their destructor to run
			producers.clear();
			break;
	}
}



/* Run a validation test on a queue specialization */
template <class Producer>
bool validate(Queue<int>& queue, unsigned repetitions, unsigned num_producers)
{
	pthread_barrier_t barrier;
	if (pthread_barrier_init(&barrier, nullptr, num_producers + 1) != 0)
	{
		return false;
	}

	fprintf(stderr, "Creating producers...");
	unsigned* count = new unsigned[num_producers];
	for (unsigned i = 0; i < num_producers; ++i)
	{
		try
		{
			producers.push_back( std::shared_ptr<Producer>( new Producer(barrier, i, queue) ) );
		}
		catch (const std::runtime_error& e)
		{
			fprintf(stderr, "FAIL\n");
			return false;
		}

		count[i] = 0;
	}
	fprintf(stderr, "DONE\n");

	fprintf(stderr, "Preparing...");
	int ret = pthread_barrier_wait(&barrier);
	if (ret != PTHREAD_BARRIER_SERIAL_THREAD && ret != 0)
	{
		producers.clear();
		pthread_barrier_destroy(&barrier);
		fprintf(stderr, "FAIL\n");
		return false;
	}	
	fprintf(stderr, "DONE\n");


	fprintf(stderr, "Running tests...");
	unsigned long i = (repetitions * num_producers) + 1;
	
	timespec start, stop;

	clock_gettime(CLOCK_REALTIME, &start);
	while (!producers.empty() && --i != 0)
	{
		int id;
		while (!producers.empty() && !queue.dequeue(id))
		{
			pthread_yield();
		}

		++count[id];
	}
	clock_gettime(CLOCK_REALTIME, &stop);
	unsigned failed = num_producers - producers.size();
	fprintf(stderr, "%s\n", failed == 0 ? "DONE" : "FAIL");

	unsigned long duration = Producer::diff(stop, start);
	fprintf(stdout, "Total duration: %.5f seconds\n", duration / (1000000000.0));
	if (failed == 0)
	{
		for (i = 0; i < num_producers; ++i)
		{
			bool result = count[i] == repetitions;
			fprintf(stdout, "    %02lu\t%s\n", 
					i, 
					result ? "\033[0;92mPASS\033[0m" : "\033[0;91mFAIL\033[0m");
			
			if (!result)
			{
				++failed;
			}
		}
	}
	fprintf(stdout, "%u out of %u tests passed.\n", (num_producers - failed), num_producers);

	delete[] count;

	return failed == 0;
}



int main(void)
{
	LockingQueue<int> queue(QUEUE_SIZE);
	return !validate<ManyToOneProducer>(queue, REPETITIONS, PRODUCERS);
}
