#include "producer.h"
#include "manytooneproducer.h"
#include "queue.h"
#include <pthread.h>



void ManyToOneProducer::run(Queue<int>& queue, const bool& running)
{
	unsigned id = getId();

	unsigned long remaining = getRepetitions() + 1;
	while (running && --remaining != 0)
	{
		while (running && !queue.enqueue(id))
		{
			pthread_yield();
		}
	}
}
