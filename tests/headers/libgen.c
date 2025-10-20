// https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/libgen.h.html

#include "core.h"

#include <libgen.h>

TEST(libgen_h)
{
	FUNCTION(basename, FN(char *, char *));
	FUNCTION(dirname, FN(char *, char *));
}
