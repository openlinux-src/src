#include <sys/socket.h>
#include <syscall.h>

int accept(int socket, struct sockaddr *restrict address,
	   socklen_t *restrict address_len)
{
	return syscall(accept, socket, address, address_len);
}
