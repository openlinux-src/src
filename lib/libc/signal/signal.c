#include <signal.h>

void (*signal(int sig, void (*func)(int)))(int)
{
	struct sigaction act, oact;

	act.sa_handler = func;
	act.sa_flags = SA_RESTART;

	if (sigaction(sig, &act, &oact) < 0)
		return SIG_ERR;

	return oact.sa_handler;
}
