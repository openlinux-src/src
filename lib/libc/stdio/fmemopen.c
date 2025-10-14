#include <io.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

FILE *fmemopen(void *restrict buf, size_t max_size, const char *restrict mode)
{
	int flags;
	FILE *f = calloc(1, sizeof(FILE));

	if (f == NULL)
		return f;

	f->fd = -1;
	f->buf = buf;
	f->buf_size = max_size;
	f->type = _IOFBF;

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

	f->flags = flags;

	__libc_fadd(f);

	return f;
}
