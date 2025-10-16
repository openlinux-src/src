#include <pwd.h>
#include <stddef.h>
#include <limits.h>

struct passwd *getpwuid(uid_t uid)
{
	static struct passwd pwd;
	static char buf[LINE_MAX * 4];
	struct passwd *res;

	if (getpwuid_r(uid, &pwd, buf, sizeof(buf), &res) != 0)
		return NULL;

	return res;
}
