#include "producer.h"
#include "locking.h"
#include "optimized.h"
#include "lamport.h"



int main(void)
{
	LockingQueue locking(QUEUE_SIZE);
	OptimizedLockingQueue locking_optimized(QUEUE_SIZE);
	LamportQueue lockfree(QUEUE_SIZE);

	unsigned failed = 0;
	failed += ! Producer::test_queue(locking, REPETITIONS, PRODUCERS);
	failed += ! Producer::test_queue(locking_optimized, REPETITIONS, PRODUCERS);
	failed += ! Producer::test_queue(lockfree, REPETITIONS, PRODUCERS);

	return failed == 0;
}
