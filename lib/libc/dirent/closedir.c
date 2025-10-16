#include <errno.h>
#include <__dirent.h>
#include <unistd.h>
#include <dirent.h>

int closedir(DIR *dirp)
{
	intptr_t i = (intptr_t)dirp;

	if (i >= 0) {
		errno = EBADF;
		return -1;
	}

	return close(~i);
}
