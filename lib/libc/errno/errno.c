#include <thread.h>
#include <threads.h>

static int __thread_errno = 0;

int *__errno_location(void)
{
	return &__thread_errno;
}
