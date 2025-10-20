// https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/iconv.h.html

#include "core.h"

#include <iconv.h>

TEST(iconv_h)
{
	TYPE(iconv_t);
	TYPE(size_t);

	FUNCTION(iconv,
		 FN(size_t, iconv_t, char **, size_t *, char **, size_t *));
	FUNCTION(iconv_close, FN(int, iconv_t));
	FUNCTION(iconv_open, FN(iconv_t, const char *, const char *));
}
