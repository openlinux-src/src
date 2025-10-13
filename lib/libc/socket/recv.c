#include <sys/socket.h>
#include <syscall.h>

ssize_t recv(int socket, void *buffer, size_t length, int flags)
{
	return syscall(recvfrom, socket, buffer, length, flags, 0, 0);
}
