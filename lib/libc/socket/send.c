#include <sys/socket.h>
#include <syscall.h>

ssize_t send(int socket, const void *buffer, size_t length, int flags)
{
	return syscall(sendto, socket, buffer, length, flags, 0, 0);
}
