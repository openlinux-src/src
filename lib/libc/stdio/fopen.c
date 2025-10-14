#include <io.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

FILE *fopen(const char *restrict pathname, const char *restrict mode)
{
	int fd, flags;
	FILE *fp;

	flags = 0;
	if (mode[0] == 'r') {
		flags = O_RDONLY;
	} else if (mode[0] == 'w') {
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	} else if (mode[0] == 'a') {
		flags = O_WRONLY | O_CREAT | O_APPEND;
	} else {
		errno = EINVAL;
		return NULL;
	}

	if (strchr(mode, '+')) {
		flags = (flags & ~(O_RDONLY | O_WRONLY)) | O_RDWR;
	}

	if ((fd = open(pathname, flags)) < 0)
		return NULL;

	if ((fp = calloc(1, sizeof(FILE))) == NULL)
		return NULL;

	fp->fd = fd;
	fp->buf_size = BUFSIZ;
	fp->flags = flags;
	fp->type = _IOLBF;
	atomic_flag_clear(&fp->lock);
	if ((fp->buf = malloc(BUFSIZ)) == NULL) {
		close(fd);
		free(fp);
		return NULL;
	}

	__libc_fadd(fp);
	return fp;
}
