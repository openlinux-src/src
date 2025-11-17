#include <syscall.h>

int umount(const char *target)
{
	return syscall(umount2, target, 0);
}
