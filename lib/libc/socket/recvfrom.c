#include <sys/socket.h>
#include <syscall.h>

ssize_t recvfrom(int socket, void *restrict buffer, size_t length, int flags,
		 struct sockaddr *restrict address,
		 socklen_t *restrict address_len)
{
	return syscall(recvfrom, socket, buffer, length, flags, address,
		       address_len);
}
