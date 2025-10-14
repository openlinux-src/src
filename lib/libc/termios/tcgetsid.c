#include <termios.h>
#include <syscall.h>
#include <asm-generic/ioctls.h>

pid_t tcgetsid(int fildes)
{
	pid_t sid = 0;

	if (syscall(ioctl, fildes, TIOCGSID, &sid) < 0)
		return -1;

	return sid;
}
