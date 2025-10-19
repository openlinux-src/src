#include <syscall.h>
#include <unistd.h>

ssize_t read(int fildes, void *buf, size_t nbyte)
{
	return syscall(read, fildes, buf, nbyte);
}
