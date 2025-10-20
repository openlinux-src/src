// https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/devctl.h.html

#include "core.h"

#include <devctl.h>

TEST(devctl_h)
{
	TYPE(size_t);

	FUNCTION(posix_devctl,
		 FN(int, int, int, void *restrict, size_t, int *restrict));
}
