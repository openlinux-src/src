#include <fcntl.h>
#include <syscall.h>
#include <sys/stat.h>

int mkdir(const char *path, mode_t mode)
{
#ifdef __NR_mkdir
	return syscall(mkdir, path, mode);
#else
	return syscall(mkdirat, AT_FDCWD, path, mode);
#endif
}
