#include "producer.h"
#include "idrepeater.h"
#include "queue.h"
#include <pthread.h>



/* Repeat the given ID */
void IdRepeater::run(Queue& queue, const bool& running)
{
	unsigned remaining = repetitions + 1;
	while (running && --remaining != 0)
	{
		while (running && !queue.enqueue(id))
		{
			pthread_yield();
		}
	}
}
