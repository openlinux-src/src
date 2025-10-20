// https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/fmtmsg.h.html

#include "core.h"

#include <fnmatch.h>

TEST(fnmatch_h)
{
	MACRO(FNM_PATHNAME)
	MACRO(FNM_NOESCAPE)
	MACRO(FNM_PERIOD)
	MACRO(FNM_CASEFOLD)
	MACRO(FNM_NOMATCH)
	MACRO(FNM_IGNORECASE)

	FUNCTION(fnmatch, FN(int, const char *, const char *, int))
}
