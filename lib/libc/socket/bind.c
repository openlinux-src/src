#include <sys/socket.h>
#include <syscall.h>

int bind(int socket, const struct sockaddr *address, socklen_t address_len)
{
	return syscall(bind, socket, address, address_len);
}
