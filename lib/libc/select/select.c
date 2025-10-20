#include <sys/select.h>
#include <syscall.h>

int select(int nfds, fd_set *restrict readfds, fd_set *restrict writefds,
	   fd_set *restrict errorfds, struct timeval *restrict timeout)
{
	return syscall(pselect6, nfds, readfds, writefds, errorfds, timeout,
		       ((syscall_arg_t[]){ 0, 8 }));
}
