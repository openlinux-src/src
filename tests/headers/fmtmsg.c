// https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/fmtmsg.h.html

#include "core.h"

#include <fmtmsg.h>

TEST(fmtmsg_h)
{
	MACRO(MM_HARD)
	MACRO(MM_SOFT)
	MACRO(MM_FIRM)
	MACRO(MM_APPL)
	MACRO(MM_UTIL)
	MACRO(MM_OPSYS)
	MACRO(MM_RECOVER)
	MACRO(MM_NRECOV)
	MACRO(MM_PRINT)
	MACRO(MM_CONSOLE)
	MACRO(MM_NOSEV)
	MACRO(MM_HALT)
	MACRO(MM_ERROR)
	MACRO(MM_WARNING)
	MACRO(MM_INFO)
	MACRO(MM_NOTOK);
	MACRO(MM_OK);
	MACRO(MM_NOMSG);
	MACRO(MM_NOCON);

	MACRO_VALUE(MM_NULLLBL, ((char *)0));
	MACRO_VALUE(MM_NULLSEV, 0);
	MACRO_VALUE(MM_NULLMC, ((char *)0));
	MACRO_VALUE(MM_NULLTXT, ((char *)0));
	MACRO_VALUE(MM_NULLACT, ((char *)0));
	MACRO_VALUE(MM_NULLTAG, ((char *)0));

	FUNCTION(fmtmsg, FN(int, long, const char *, int, const char *,
			    const char *, const char *));
}
