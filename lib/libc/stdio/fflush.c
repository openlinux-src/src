#include <stdio.h>
#include <unistd.h>
#include <atomic.h>
#include <io.h>
#include <errno.h>
#include <string.h>

int fflush(FILE *stream)
{
	// Handle NULL stream - flush all open streams
	if (stream == NULL) {
		// TODO: Implement flushing all open streams
		// For now, just return success
		return 0;
	}

	// Nothing to flush if buffer is empty
	if (stream->buf_len == 0) {
		return 0;
	}

	// Handle special case of invalid file descriptor
	if (stream->fd == -1) {
		stream->buf_len = 0;
		return 0;
	}

	// Check if stream is in error state
	if (stream->flags & _IO_ERR) {
		errno = EIO;
		return EOF;
	}

	// Check if stream is writable
	if ((stream->flags & O_ACCMODE) == O_RDONLY) {
		errno = EBADF;
		return EOF;
	}

	LIBC_LOCK(stream->lock);

	size_t bytes_to_write = stream->buf_len;
	size_t total_written = 0;
	char *buf_ptr = stream->buf;

	// Write all buffered data
	while (total_written < bytes_to_write) {
		ssize_t result = write(stream->fd, buf_ptr + total_written,
				       bytes_to_write - total_written);

		if (result < 0) {
			// Write error occurred
			stream->flags |= _IO_ERR;
			LIBC_UNLOCK(stream->lock);
			return EOF;
		}

		if (result == 0) {
			// No progress made (shouldn't happen with regular files)
			break;
		}

		total_written += result;
	}

	// Update buffer state
	if (total_written == bytes_to_write) {
		// All data was written successfully
		stream->buf_len = 0;
		stream->buf_pos = 0;
	} else {
		// Partial write - move remaining data to beginning of buffer
		size_t remaining = bytes_to_write - total_written;
		memmove(stream->buf, stream->buf + total_written, remaining);
		stream->buf_len = remaining;
		stream->buf_pos = 0;
	}

	LIBC_UNLOCK(stream->lock);

	// Return success if all data was written, error otherwise
	return (total_written == bytes_to_write) ? 0 : EOF;
}
