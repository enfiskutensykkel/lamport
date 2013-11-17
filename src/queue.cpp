#include "queue.h"
#include <stdexcept>
#include <cstdint>



/* Helper function to round up a number to the nearest power of two */
static uint32_t nearest_pow(uint32_t i)
{
	uint32_t pos, leading_zeros;
	for (pos = 1 << 31, leading_zeros = 0; (pos & i) == 0 && leading_zeros < 32; pos >>= 1, ++leading_zeros);

	return 1 << (32 - leading_zeros);
}



/* Construct a queue object and allocate the queue buffer */
Queue::Queue(uint32_t slots)
	: capacity(nearest_pow(slots - 1))
	, head(0), tail(0)
	, buffer(nullptr)
{
	buffer = new int[capacity];
	if (buffer == nullptr)
	{
		throw std::runtime_error("failed to allocate queue buffer");
	}
}



/* Destroy the queue object and free the queue buffer */
Queue::~Queue()
{
	delete[] buffer;
}



/* Check if a queue is empty (it has no elements) */
bool Queue::empty()
{
	return size() == 0;
}
