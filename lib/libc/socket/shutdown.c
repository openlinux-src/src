#include <syscall.h>

int shutdown(int socket, int how)
{
	return syscall(shutdown, socket, how);
}
