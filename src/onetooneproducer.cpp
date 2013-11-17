#include "producer.h"
#include "onetooneproducer.h"
#include "queue.h"
#include <pthread.h>



void OneToOneProducer::run(Queue<int>& queue, const bool& running)
{
	unsigned long i = getRepetitions() + 1;
	while (running && --i != 0)
	{
		while (running && !queue.enqueue(i))
		{
			pthread_yield();
		}
	}
}
