#ifndef __LOCKING_QUEUE_H__
#define __LOCKING_QUEUE_H__

#include "queue.h"
#include <string>
#include <pthread.h>


template <typename T = int>
class LockingQueue : public Queue<T>
{
	public:
		explicit LockingQueue(unsigned slots);

		virtual ~LockingQueue();

		virtual std::string getName()
		{
			return "LockingQueue";
		};

		virtual bool enqueue(const T& element);

		virtual bool dequeue(T& element);

		virtual unsigned size();

		virtual bool empty();

	private:
		pthread_mutex_t lock;
};

#endif
