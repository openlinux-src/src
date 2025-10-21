#include <signal.h>
#include <syscall.h>

#define _NSIG 64

int sigtimedwait(const sigset_t *restrict set, siginfo_t *restrict info,
		 const struct timespec *restrict timeout)
{
	return syscall(rt_sigtimedwait, set, info, timeout, _NSIG / 8);
}
