#include <stdio.h>
#include <fcntl.h>
#include <syscall.h>

int remove(const char *path)
{
	if (syscall(unlinkat, AT_FDCWD, path, 0) < 0) {
		if (errno == EISDIR) {
			return syscall(unlinkat, AT_FDCWD, path, AT_REMOVEDIR);
		} else {
			return -1;
		}
	}

	return 0;
}
