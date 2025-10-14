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
		extern char __stdout_buffer[];
		extern char __stderr_buffer[];
		extern char __stdin_buffer[];

		if (fp->buf != __stdout_buffer && fp->buf != __stderr_buffer &&
		    fp->buf != __stdin_buffer) {
			free(fp->buf);
		}
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
