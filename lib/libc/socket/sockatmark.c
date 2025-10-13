#include <syscall.h>
#include <asm-generic/sockios.h>

int sockatmark(int s)
{
	int arg;
	return syscall(ioctl, s, SIOCATMARK, &arg) < 0 ? -1 : arg;
}
