#include <asm/vdso.h>
#include <syscall.h>
#include <time.h>

int clock_gettime(clockid_t clock_id, struct timespec *tp)
{
#if defined(__x86_64__)
	if (__vdso_clock_gettime)
		return __vdso_clock_gettime(clock_id, tp);
#endif

	return syscall(clock_gettime, clock_id, tp);
}
