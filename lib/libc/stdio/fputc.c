#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <io.h>

int fputc(int c, FILE *stream)
{
	if (!stream) {
		errno = EINVAL;
		return EOF;
	}

	// Special case for string buffer operations (snprintf)
	// When fd is -1, we're writing to a string buffer
	if (stream->fd == -1 && stream->buf != NULL) {
		// Check if there's space (leave room for null terminator)
		if (stream->buf_len >= stream->buf_size - 1) {
			return EOF;
		}

		stream->buf[stream->buf_len++] = (char)c;
		return (unsigned char)c;
	}

	// For regular file operations, use fwrite
	return fwrite(&c, 1, 1, stream) ? c : EOF;
}
