#include <io.h>
#include <atomic.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

size_t fwrite(const void *restrict ptr, size_t size, size_t nitems,
	      FILE *restrict stream)
{
	size_t total_bytes;
	size_t written = 0;
	const char *data = (const char *)ptr;

	// Validate parameters
	if (!ptr || !stream || size == 0) {
		if (size == 0)
			return nitems;
		errno = EINVAL;
		return 0;
	}

	// Special case for string buffer operations (snprintf)
	// When fd is -1, we're writing to a string buffer
	if (stream->fd == -1 && stream->buf != NULL) {
		// Calculate total bytes with overflow check
		if (__builtin_mul_overflow(size, nitems, &total_bytes)) {
			errno = EOVERFLOW;
			return 0;
		}

		// Check remaining buffer space (leave room for null terminator)
		size_t space_left =
			stream->buf_size > stream->buf_len ?
				stream->buf_size - stream->buf_len - 1 :
				0;

		if (space_left == 0) {
			return 0;
		}

		// Copy as much as fits
		size_t to_copy = total_bytes < space_left ? total_bytes :
							    space_left;

		if (to_copy > 0) {
			memcpy(stream->buf + stream->buf_len, data, to_copy);
			stream->buf_len += to_copy;
		}

		// Return number of complete items written
		return to_copy == total_bytes ? nitems : to_copy / size;
	}

	// Calculate total bytes with overflow check
	if (__builtin_mul_overflow(size, nitems, &total_bytes)) {
		errno = EOVERFLOW;
		return 0;
	}

	// Check if stream is writable (fix the flag check)
	if ((stream->flags & O_ACCMODE) == O_RDONLY) {
		errno = EBADF;
		return 0;
	}

	// Check for error state
	if (stream->flags & _IO_ERR) {
		errno = EIO;
		return 0;
	}

	LIBC_LOCK(stream->lock);

	// Handle unbuffered I/O
	if (stream->type == _IONBF) {
		ssize_t result = write(stream->fd, data, total_bytes);
		LIBC_UNLOCK(stream->lock);

		if (result < 0) {
			stream->flags |= _IO_ERR;
			return 0;
		}

		return result == (ssize_t)total_bytes ? nitems : result / size;
	}

	// Handle buffered I/O (both _IOFBF and _IOLBF)
	size_t remaining = total_bytes;

	while (remaining > 0) {
		// Check if buffer is full
		size_t space_available = stream->buf_size - stream->buf_len;

		if (space_available == 0) {
			// Flush the buffer
			LIBC_UNLOCK(stream->lock);
			if (fflush(stream) != 0) {
				return written / size;
			}
			space_available = stream->buf_size;
		}

		// Determine how much to copy this iteration
		size_t to_copy = remaining < space_available ? remaining :
							       space_available;

		// Copy data to buffer
		memcpy(stream->buf + stream->buf_len, data + written, to_copy);
		stream->buf_len += to_copy;
		written += to_copy;
		remaining -= to_copy;

		// For line buffered streams, check if we need to flush
		if (stream->type == _IOLBF) {
			// Look for newlines in the data we just added
			char *newline_pos =
				memchr(stream->buf + stream->buf_len - to_copy,
				       '\n', to_copy);
			if (newline_pos != NULL) {
				LIBC_UNLOCK(stream->lock);
				if (fflush(stream) != 0) {
					return written / size;
				}
			}
		}

		// For fully buffered streams, flush if buffer is full
		else if (stream->type == _IOFBF &&
			 stream->buf_len == stream->buf_size) {
			LIBC_UNLOCK(stream->lock);
			if (fflush(stream) != 0) {
				return written / size;
			}
		}
	}

	LIBC_UNLOCK(stream->lock);

	// Return number of complete items written
	return written == total_bytes ? nitems : written / size;
}
