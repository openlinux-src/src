#include <syscall.h>
#include <sys/mount.h>

int mount(const char *source, const char *target, const char *filesystemtype,
	  unsigned long mountflags, const void *_Nullable data)
{
	return syscall(mount, source, target, filesystemtype, mountflags, data);
}
