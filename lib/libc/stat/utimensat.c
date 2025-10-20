#include <time.h>
#include <syscall.h>

int utimensat(int fd, const char *path, const struct timespec times[2],
	      int flag)
{
	return syscall(utimensat, fd, path, times, flag);
}
