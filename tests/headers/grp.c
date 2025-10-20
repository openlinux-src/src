// https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/glob.h.html

#include "core.h"

#include <grp.h>

TEST(grp_h)
{
	TYPE(struct group);
	STRUCT_MEMBER(struct group, char *, gr_name);
	STRUCT_MEMBER(struct group, gid_t, gr_gid);
	STRUCT_MEMBER(struct group, char **, gr_mem);

	TYPE(gid_t);
	TYPE(size_t);

	FUNCTION(endgrent, FN(void, void));
	FUNCTION(getgrent, FN(struct group *, void));
	FUNCTION(getgrgid, FN(struct group *, gid_t));
	FUNCTION(getgrgid_r, FN(int, gid_t, struct group *, char *, size_t,
				struct group **));
	FUNCTION(getgrnam, FN(struct group *, const char *));
	FUNCTION(getgrnam_r, FN(int, const char *, struct group *, char *,
				size_t, struct group **));
	FUNCTION(setgrent, FN(void, void));
}
