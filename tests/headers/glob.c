// https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/glob.h.html

#include "core.h"

#include <glob.h>

TEST(glob_h)
{
	TYPE(glob_t);
	STRUCT_MEMBER(glob_t, size_t, gl_pathc);
	STRUCT_MEMBER(glob_t, char **, gl_pathv);
	STRUCT_MEMBER(glob_t, size_t, gl_offs);

	TYPE(size_t);

	MACRO(GLOB_APPEND);
	MACRO(GLOB_DOOFFS);
	MACRO(GLOB_ERR);
	MACRO(GLOB_MARK);
	MACRO(GLOB_NOCHECK);
	MACRO(GLOB_NOESCAPE);
	MACRO(GLOB_NOSORT);

	MACRO(GLOB_ABORTED);
	MACRO(GLOB_NOMATCH);
	MACRO(GLOB_NOSPACE);

	FUNCTION(glob, FN(int, const char *, int, int (*)(const char *, int),
			  glob_t *));

	FUNCTION(globfree, FN(void, glob_t *));
}
