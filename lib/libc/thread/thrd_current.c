#include <thread.h>
#include <threads.h>
#include <unistd.h>

_Thread_local struct __thread_self self = { 0 };

thrd_t thrd_current(void)
{
	return &self;
}
