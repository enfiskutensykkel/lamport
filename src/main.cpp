#include "producer.h"
#include "lockingqueue.h"
#include <vector>
#include <memory>
#include <stdexcept>
#include <pthread.h>
#include <signal.h>



static std::vector<Producer> producers;



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
void validate(unsigned repetitions, Queue<int>& queue, unsigned num_producers)
{
	pthread_barrier_t barrier;
	if (pthread_barrier_init(&barrier, nullptr, num_producers + 1) != 0)
	{
		throw std::runtime_error("failed to initialize barrier");
	}

	unsigned* count = new unsigned[num_producers];
	for (unsigned i = 0; i < num_producers; ++i)
	{
		count[i] = 0;
	}

	int ret = pthread_barrier_wait(&barrier);
	if (ret != PTHREAD_BARRIER_SERIAL_THREAD && ret != 0)
	{
		producers.clear();
		pthread_barrier_destroy(&barrier);
		throw std::runtime_error("failed to wait on barrier");
	}	

	unsigned long i = (repetitions * num_producers) + 1;
	while (!producers.empty() && --i != 0)
	{
		int id;
		while (!producers.empty() && !queue.dequeue(id))
		{
			pthread_yield();
		}

		++count[id];
	}

	delete[] count;
}



int main(void)
{
	return 0;
}
