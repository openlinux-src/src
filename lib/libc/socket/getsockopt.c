#include <sys/socket.h>
#include <syscall.h>

int getsockopt(int socket, int level, int option_name,
	       void *restrict option_value, socklen_t *restrict option_len)
{
	return syscall(getsockopt, socket, level, option_name, option_value,
		       option_len);
}
