#include <sys/socket.h>
#include <syscall.h>

ssize_t sendto(int socket, const void *message, size_t length, int flags,
	       const struct sockaddr *dest_addr, socklen_t dest_len)
{
	return syscall(sendto, socket, message, length, flags, dest_addr,
		       dest_len);
}
