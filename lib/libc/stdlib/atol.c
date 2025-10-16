#include <stdlib.h>

long atol(const char *nptr)
{
	return strtol(nptr, (char **)NULL, 10);
}
