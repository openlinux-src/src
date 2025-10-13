#include <syscall.h>

int socketpair(int domain, int type, int protocol, int socket_vector[2])
{
	return syscall(socketpair, domain, type, protocol, socket_vector);
}
