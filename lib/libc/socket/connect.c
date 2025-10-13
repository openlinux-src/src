#include <sys/socket.h>
#include <syscall.h>

int connect(int socket, const struct sockaddr *address, socklen_t address_len)
{
	return syscall(connect, socket, address, address_len);
}
