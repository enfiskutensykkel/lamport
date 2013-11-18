#include "queue.h"
#include "nonlockingqueue.h"
#include <cstdint>



/* An atomic compare-and-swap operation */
static inline bool CAS(uint64_t& value, uint64_t expected_value, uint64_t updated_value) __attribute__((always_inline));
bool CAS(uint64_t& val, uint64_t expected, uint64_t updated)
{
	uint8_t ret;
	__asm__ volatile (
			"lock;"
			"cmpxchgq %2, %1;"
			"sete %0;"
			: "=q" (ret), "=m" (val)
			: "r" (updated), "m" (val), "a" (expected)
			: "memory");
	return ret != 0;
}



/* Create a NonLocking's queue */
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
	uint64_t curr_tail, curr_head;

	do
	{
		curr_head = head;
		curr_tail = tail;

		if (curr_tail >= curr_head + capacity)
		{
			return false;
		}
	}
	while (!CAS(tail, curr_tail, curr_tail + 1));

	buffer[curr_tail & mask] = new int(element);
	return true;
}



/* Dequeue an element */
bool LamportQueue::dequeue(int& element)
{
	uint64_t curr_head = head;
	if (curr_head == tail)
	{
		return false;
	}

	int* ptr;
	do
	{
		ptr = buffer[curr_head & mask];
	} 
	while (ptr == nullptr);

	element = *ptr;

	delete buffer[curr_head & mask];
	buffer[curr_head & mask] = nullptr;

	head = curr_head + 1;

	return true;
}



/* Return the number of enqueued elements */
uint32_t LamportQueue::size()
{
	return 0;
}
