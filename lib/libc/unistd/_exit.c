#include <syscall.h>

void _exit(int status)
{
	__syscall(exit, status);
	__builtin_unreachable();
}
