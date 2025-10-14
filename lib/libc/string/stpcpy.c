#include <string.h>

char *stpcpy(char *restrict s1, const char *restrict s2)
{
	while ((*s1++ = *s2++))
		;
	return s1 - 1;
}
