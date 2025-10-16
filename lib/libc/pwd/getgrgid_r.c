#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

static char *copy_str(const char *src, char **buf, size_t *buflen)
{
	size_t len = strlen(src) + 1;
	if (len > *buflen)
		return NULL;
	char *dst = *buf;
	memcpy(dst, src, len);
	*buf += len;
	*buflen -= len;
	return dst;
}

int getpwuid_r(uid_t uid, struct passwd *pwd, char *buf, size_t buflen,
	       struct passwd **result)
{
	FILE *stream = fopen("/etc/passwd", "r");
	if (stream == NULL) {
		*result = NULL;
		return errno;
	}

	char line[LINE_MAX];

	while (fgets(line, sizeof(line), stream)) {
		char *name = strtok(line, ":");
		char *passwd = strtok(NULL, ":");
		char *uid_str = strtok(NULL, ":");
		char *gid_str = strtok(NULL, ":");
		char *gecos = strtok(NULL, ":");
		char *dir = strtok(NULL, ":");
		char *shell = strtok(NULL, "\n");

		if (name == NULL || passwd == NULL || uid_str == NULL ||
		    gid_str == NULL)
			continue;

		uid_t file_uid = (uid_t)strtoul(uid_str, NULL, 10);
		if (file_uid != uid)
			continue;

		size_t len;
		char *p = buf;
		size_t rem = buflen;

		pwd->pw_name = copy_str(name, &buf, &rem);
		pwd->pw_dir = copy_str(dir ? dir : "", &buf, &rem);
		pwd->pw_shell = copy_str(shell ? shell : "", &buf, &rem);
		pwd->pw_uid = file_uid;
		pwd->pw_gid = (gid_t)strtoul(gid_str, NULL, 10);
		pwd->pw_uid = file_uid;
		pwd->pw_gid = (gid_t)strtoul(gid_str, NULL, 10);

		if (pwd->pw_name == NULL || pwd->pw_dir == NULL ||
		    pwd->pw_shell == NULL)
			return ERANGE;

		fclose(stream);
		*result = pwd;
		return 0;
	}

	fclose(stream);
	*result = NULL;
	return 0;
}
