#include <stddef.h>

int strncmp(const char *s1, const char *s2, size_t n)
{
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;

	while (n-- > 0) {
		if (*p1 != *p2 || *p1 == '\0')
			return *p1 - *p2;
		p1++;
		p2++;
	}

	return 0;
}
