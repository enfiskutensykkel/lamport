Lamport's queue
===============

I was inspired to do this small project after doing a workshop on 
*lock-free and high-performance concurrent algorithms* held by [Martin Thompson](https://github.com/mjpt777 "mjpt777")
at the 2013 San Francisco QCon. The workshop was focused on Java, but it 
inspired me to try it out in C++ and some inline assembly.


What is it?
-----------

This project is simply an implementation of two bounded-buffer queues for
the *producer-consumer* problem, one which uses POSIX mutexes and one
implemented as a [Lamport's queue](http://en.wikipedia.org/wiki/Lamport%27s_bakery_algorithm)
using an atomic compare-and-swap subroutine. I'm also doing a validation
test (by simply letting one producer insert a range of numbers and reading 
them out again) and a performance test (by having multiple producers compete
over the queue). The test scenarios and the idea itself are blatently ripped 
off from the workshop, but the code is written entirely by me.


How can I run it?
-----------------

To run this you need an 64-bits Intel x86, an OS that supports POSIX threads
and be able to build and compile using GNU Make and GNU C++11 (i.e. use GCC).

You should also tweak the number of concurrent threads to how many CPUs you 
have, alternatively have a CPU that supports hyperthreading. If you don't do
this, running the performance test will be pointless. I'm defaulting to 4 
threads, since this can run on a dual-core machine that supports 
hyperthreading.


Limitations
-----------

From a code perspective, I haven't really had time to look into a decent
testing framework. I will probably separate and create better tests for this
in the future, but this was really about doing a contention queue without locks
more than anything else.

From a concurrency point of view, I haven't tested it with more than one
consumer. By scoping it to just one consumer, I've done an "optimized" version
of the queue using mutexes, by only locking the tail pointer.


Other optimisations
-------------------

It's pretty apparent that doing `mod` and `div` has a high performance impact,
because of the architecture's inability to pipeline these instructions and that
these instructions demand a lot of CPU cycles. For the two "optimised" queues, 
I have therefore rounded the buffer size up to the  nearest power of two and 
use a bitwise mask instead to calculate the buffer index by division. This 
alone improves performance drastically. I haven't done any other optimizations,
but of course inlining functions, reducing memory access and references (and 
trying to hit the cache more often), tuning and tweaking compiler flags etc 
would also be fun to do at some point.

**Note:** If you turn on too aggressive compiler optimizations (like `-O3`),
weird things start to happen.
