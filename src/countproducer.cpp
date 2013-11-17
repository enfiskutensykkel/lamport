#include "producer.h"
#include "countproducer.h"
#include "queue.h"
#include <pthread.h>



/* Produce a counting range from limit to 0 */
void CountProducer::run(Queue& queue, const bool& running)
{
	unsigned i = repetitions + 1;
	while (running && --i != 0)
	{
		while (running && !queue.enqueue(i))
		{
			pthread_yield();
		}
	}
}
