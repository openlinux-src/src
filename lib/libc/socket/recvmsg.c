#include <sys/socket.h>
#include <syscall.h>

ssize_t recvmsg(int socket, struct msghdr *message, int flags)
{
	return syscall(recvmsg, socket, message, flags);
}
