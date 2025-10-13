#include <sys/socket.h>
#include <syscall.h>

int getsockname(int socket, struct sockaddr *restrict address,
		socklen_t *restrict address_len)
{
	return syscall(getsockname, socket, address, address_len);
}
