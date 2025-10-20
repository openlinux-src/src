#include <sys/select.h>
#include <stdint.h>
#include <syscall.h>

int pselect(int nfds, fd_set *restrict readfds, fd_set *restrict writefds,
	    fd_set *restrict errorfds, const struct timespec *restrict timeout,
	    const sigset_t *restrict sigmask)
{
	return syscall(pselect6, nfds, readfds, writefds, errorfds, timeout,
		       ((syscall_arg_t[]){ (uintptr_t)sigmask, 8 }));
}
