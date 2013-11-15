#include "producer.h"
#include "lockingqueue.h"
#include <vector>
#include <shared
#include <pthread.h>



void test_locking_queue(unsigned producers)
{
	pthread_barrier_t barrier;
	pthread_barrier_init(&barrier, nullptr, producers + 1);

	for (unsigned i = 0; i < producers; ++i)
	{
	}
}



int main(void)
{


	return 0;
}
