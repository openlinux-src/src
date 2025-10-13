#include <ctype.h>
#include <stddef.h>

int strncasecmp(const char *s1, const char *s2, size_t n)
{
	unsigned char c1, c2;

	if (n == 0)
		return 0;

	while (n-- > 0 && *s1 && *s2) {
		c1 = (unsigned char)tolower((unsigned char)*s1);
		c2 = (unsigned char)tolower((unsigned char)*s2);
		if (c1 != c2)
			return c1 - c2;
		s1++;
		s2++;
	}

	if (n == (size_t)-1)
		return 0;

	return (unsigned char)tolower((unsigned char)*s1) -
	       (unsigned char)tolower((unsigned char)*s2);
}
