#include <thread.h>
#include <threads.h>
#include <unistd.h>

thrd_t thrd_current(void)
{
	static _Thread_local struct __thread_self self = { 0 };
	return &self;
}
