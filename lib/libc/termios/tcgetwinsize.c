#include <termios.h>
#include <syscall.h>
#include <asm-generic/ioctls.h>

struct __winsize {
	unsigned short ws_row;
	unsigned short ws_col;
	unsigned short ws_xpixel;
	unsigned short ws_ypixel;
};

int tcgetwinsize(int fildes, struct winsize *winsize_p)
{
	long ret;
	struct __winsize winsize = { 0 };

	ret = syscall(ioctl, fildes, TIOCGWINSZ, &winsize);

	winsize_p->ws_row = winsize.ws_row;
	winsize_p->ws_col = winsize.ws_col;

	return ret;
}
