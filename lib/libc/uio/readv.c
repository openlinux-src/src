#include <unistd.h>
#include <linux/uio.h>
#include <syscall.h>
#include <errno.h>
#include <string.h>

ssize_t readv(int fd, const struct iovec *iov, int iovcnt)
{
	ssize_t total = 0;

	if (iovcnt == 0) {
		return 0;
	}

	if (iovcnt < 0) {
		errno = EINVAL;
		return -1;
	}

	while (iovcnt > 0) {
		int chunk = iovcnt > UIO_MAXIOV ? UIO_MAXIOV : iovcnt;

		struct iovec local[UIO_MAXIOV];
		memcpy(local, iov, chunk * sizeof(struct iovec));

		ssize_t ret = syscall(readv, fd, local, chunk);
		if (ret < 0) {
			if (total > 0 && (errno == EAGAIN || errno == EINTR))
				return total;
			return ret;
		}
		if (ret == 0)
			break; // EOF

		total += ret;

		ssize_t remaining = ret;
		while (remaining > 0 && iovcnt > 0) {
			if ((size_t)remaining >= iov->iov_len) {
				remaining -= iov->iov_len;
				iov++;
				iovcnt--;
			} else {
				struct iovec tmp = *iov;
				tmp.iov_base = (char *)tmp.iov_base + remaining;
				tmp.iov_len -= remaining;
				memcpy(local, &tmp, sizeof(tmp));
				remaining = 0;
				break;
			}
		}
	}

	return total;
}
