#include <syscall.h>

int umount2(const char *target, int flags)
{
	return syscall(umount2, target, flags);
}
