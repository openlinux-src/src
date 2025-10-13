#include <sys/socket.h>
#include <syscall.h>

int accept4(int socket, struct sockaddr *restrict address,
	    socklen_t *restrict address_len, int flag)
{
	return syscall(accept4, socket, address, address_len, flag);
}
