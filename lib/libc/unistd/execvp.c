#include <errno.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

extern char **environ;

int execvp(const char *file, char *const argv[])
{
	if (*file == '\0') {
		errno = ENOENT;
		return -1;
	}

	if (strchr(file, '/') != NULL) {
		return execv(file, argv);
	} else {
		char *ptr;
		char *path = NULL; // becouse we dont have getenv

		if (path == NULL || *path == '\0') {
			path = "/bin";
		}

		do {
			char buf[PATH_MAX];

			if ((ptr = strchr(path, ':')) == NULL) {
				if (snprintf(buf, PATH_MAX, "%s/%s", path,
					     file) >= PATH_MAX) {
					errno = ENAMETOOLONG;
					return -1;
				}

				execv(buf, argv);
				break;
			}

			if (snprintf(buf, PATH_MAX, "%.*s/%s",
				     (int)(ptr - path), path,
				     file) >= PATH_MAX) {
				errno = ENAMETOOLONG;
				return -1;
			}

			execv(buf, argv);
		} while (*ptr != '\0');

		errno = ENOENT;
		return -1;
	}

	return 0;
}
