#include <sys/socket.h>
#include <syscall.h>

int getpeername(int socket, struct sockaddr *restrict address,
		socklen_t *restrict address_len)
{
	return syscall(getpeername, socket, address, address_len);
}
