#include <stdio.h>
#include <unistd.h>
#include <atomic.h>
#include <io.h>

int fflush(FILE *stream)
{
	if (stream->buf_len == 0)
		return 0;

	if (stream->fd == -1) {
		stream->buf_len = 0;
	}

	LIBC_LOCK(stream->lock);
	ssize_t w = write(stream->fd, stream->buf, stream->buf_len);

	if (w < 0) {
		LIBC_UNLOCK(stream->lock);
		return -1;
	}

	stream->buf_len -= w;
	LIBC_UNLOCK(stream->lock);
	return 0;
}
