#include <errno.h>
#include <__dirent.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

DIR *fdopendir(int fildes)
{
	if (fildes < 0) {
		errno = EBADF;
		return NULL;
	}

	return (DIR *)(intptr_t)~fildes;
}
