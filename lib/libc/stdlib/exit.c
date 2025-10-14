#include <io.h>
#include <unistd.h>
#include <stdio.h>

static void file_free(FILE *fp)
{
	if (fp == NULL) {
		return;
	}

	fflush(fp);

	if (fp->fd > STDERR_FILENO) {
		close(fp->fd);
	}

	if (fp->buf) {
		free(fp->buf);
	}

	if (fp != stdout && fp != stderr && fp != stdin)
		free(fp);
}

void exit(int status)
{
	fflush(stdout);

	if (stdout->next) {
		FILE *cur = stdout->next;
		while (cur) {
			struct __FILE *next = cur->next;
			file_free(cur);
			cur = next;
		}
	}

	_exit(status);
}
