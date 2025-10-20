#include <fcntl.h>
#include <errno.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/time.h>

int utimes(const char *path, const struct timeval times[2])
{
	struct timespec ts[2];

	if (times) {
		if (times[0].tv_usec >= 1000000ULL ||
		    times[1].tv_usec >= 1000000ULL) {
			errno = EINVAL;
			return -1;
		}

		ts[0].tv_sec = times[0].tv_sec;
		ts[0].tv_nsec = times[0].tv_usec * 1000;
		ts[1].tv_sec = times[1].tv_sec;
		ts[1].tv_nsec = times[1].tv_usec * 1000;
	}

	return utimensat(AT_FDCWD, path, times ? ts : NULL, 0);
}
