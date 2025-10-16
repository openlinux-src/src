#include <stdlib.h>

long long atoll(const char *nptr)
{
	return strtoll(nptr, (char **)NULL, 10);
}
