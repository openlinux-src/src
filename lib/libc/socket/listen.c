#include <sys/socket.h>
#include <syscall.h>

int listen(int socket, int backlog)
{
	return syscall(listen, backlog);
}
