#include "producer.h"
#include "locking.h"
#include "optimized.h"
#include "lamport.h"
#include <cstdio>
#include <signal.h>



int main(void)
{
	LockingQueue locking(QUEUE_SIZE);
	OptimizedLockingQueue locking_optimized(QUEUE_SIZE);
	LamportQueue lockfree(QUEUE_SIZE);

	Producer::test_queue(locking, REPETITIONS, PRODUCERS);
	Producer::test_queue(locking_optimized, REPETITIONS, PRODUCERS);
	Producer::test_queue(lockfree, REPETITIONS, PRODUCERS);

	return 0;
}
