#include "queue.h"
#include "lamportqueue.h"
#include <cstdint>



/* Create a Lamport's queue */
LamportQueue::LamportQueue(uint32_t slots)
	: Queue(slots), mask(capacity - 1)
{
}



/* Destroy the queue and free resources */
LamportQueue::~LamportQueue()
{
}



/* Enqueue an element */
bool LamportQueue::enqueue(int element)
{
	return false;
}



/* Dequeue an element */
bool LamportQueue::dequeue(int& element)
{
	return false;
}



/* Return the number of enqueued elements */
uint32_t LamportQueue::size()
{
	return 0;
}
