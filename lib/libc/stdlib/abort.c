#include <atomic.h>
#include <stdlib.h>
#include <syscall.h>
#include <asm-generic/signal.h>
#include <thread.h>
#include <threads.h>
#include <unistd.h>
#include <libc.h>

int raise(int);

_Noreturn void abort(void)
{
	struct sigaction sa;

	raise(SIGABRT);

	LIBC_LOCK(libc.lock.abort);
	sa.sa_handler = SIG_DFL;

	__syscall(rt_sigaction, SIGABRT, &sa, NULL, _NSIG / 8);
	__syscall(tkill, thrd_current()->tid, SIGABRT);

	// This point should never be reached
	raise(SIGKILL);
	_exit(127);
}
