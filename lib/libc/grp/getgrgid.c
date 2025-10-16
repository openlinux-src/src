#include <grp.h>
#include <stddef.h>
#include <limits.h>

struct group *getgrgid(gid_t gid)
{
	static struct group grp;
	static char buf[LINE_MAX * 2];
	struct group *res;

	if (getgrgid_r(gid, &grp, buf, sizeof(buf), &res) != 0)
		return NULL;

	return res;
}
