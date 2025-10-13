#include <syscall.h>

int socket(int domain, int type, int protocol)
{
	return syscall(socket, domain, type, protocol);
}
