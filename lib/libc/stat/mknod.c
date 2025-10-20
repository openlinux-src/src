#include <fcntl.h>
#include <syscall.h>
#include <sys/stat.h>

int mknod(const char *path, mode_t mode, dev_t dev)
{
	return syscall(mknodat, AT_FDCWD, path, mode, dev);
}
