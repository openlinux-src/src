#include <io.h>
#include <unistd.h>
#include <stdio.h>

FILE *fdopen(int fildes, const char *mode)
{
	FILE *fp;

	if (mode == NULL ||
	    (mode[0] != 'r' && mode[0] != 'w' && mode[0] != 'a')) {
		return NULL;
	}

	if ((fp = calloc(1, sizeof(FILE))) == NULL)
		return NULL;

	fp->fd = fildes;
	atomic_flag_clear(&fp->lock);
	if (mode[0] == 'r') {
		fp->type = _IONBF;
	} else if (mode[0] == 'w') {
		fp->type = _IOLBF;
	} else if (mode[0] == 'a') {
		fp->type = _IONBF;
		off_t offset = lseek(fildes, 0, SEEK_END);
		if (offset == (off_t)-1) {
			free(fp);
			return NULL;
		}
	}

	__libc_fadd(fp);
	return fp;
}
