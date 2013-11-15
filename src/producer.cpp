#include "thread.h"
#include <pthread.h>
#include <stdexcept>



Thread::Thread(pthread_barrier_t& b, unsigned id, Queue<int> q)
	: barrier(b), queue(q), id(id), running(false)
{
	if (pthread_create(&thread, nullptr, (void* (*)(void*)) &Thread::dispatch, static_cast<void*>(this)) != 0)
	{
		throw std::runtime_error("failed to create thread");
	}		
}



Thread::~Thread()
{
	running = false;
	if (pthread_join(thread, nullptr) != 0)
	{

	}
}



void Producer::run()
{
	if (pthread_barrier_wait(&barrier) != 0)
	{
		// TODO: Do something clever
		return;
	}

	unsigned long remaining = REPETITIONS + 1;
	while (running && --remaining != 0)
	{
		while (running && !queue.enqueue(id))
		{
			pthread_yield();
		}
	}
}



void Thread::dispatch(Thread* thread)
{
	if (thread->running)
	{
		thread->run();
	}
}
