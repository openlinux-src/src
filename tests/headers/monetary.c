// https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/monetary.h.html

#include "core.h"

#include <monetary.h>

TEST(monetary_h)
{
	TYPE(locale_t);
	TYPE(size_t);
	TYPE(ssize_t);

	FUNCTION(strfmon, FN(ssize_t, char *restrict, size_t,
			     const char *restrict, ...));
	FUNCTION(strfmon_l, FN(ssize_t, char *restrict, size_t, locale_t,
			       const char *restrict, ...));
}
