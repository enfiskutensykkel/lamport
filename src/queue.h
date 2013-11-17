#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <cstdint>
#include <string>


template <typename T>
class Queue
{
	public:
		explicit Queue(unsigned slots)
			: capacity(1 << (32 - leading_zeros((uint32_t) slots - 1)))
			, head(0), tail(0)
		{
			queue = new T*[capacity];

			for (unsigned i = 0; i < capacity; ++i)
			{
				queue[i] = nullptr;
			}
		};



		virtual ~Queue()
		{
			for (unsigned i = 0; i < capacity; ++i)
			{
				if (queue[i] != nullptr)
				{
					delete queue[i];
				}
			}

			delete[] queue;
		};

		
		virtual std::string getName() = 0;

		virtual bool enqueue(const T& element) = 0;

		virtual bool dequeue(T& element) = 0;

		virtual unsigned size() = 0;

		virtual bool empty() = 0;

		uint32_t getCapacity()
		{
			return capacity;
		};

	protected:
		const uint32_t capacity;
		uint64_t head;
		uint64_t tail;

		T** queue;

	private:
		static uint32_t leading_zeros(uint32_t i)
		{
			uint32_t p, c;
			for (p = 1 << 31, c = 0; (p & i) == 0 && c < 32; p = p >> 1, ++c);
			return c;
		};
};

#endif
