#include <string.h>
#include <stdint.h>
#include <limits.h>

#define ALIGN (sizeof(size_t))
#define ONES ((size_t)-1 / UCHAR_MAX)
#define HIGHS (ONES * (UCHAR_MAX / 2 + 1))
#define HASZERO(x) ((x) - ONES & ~(x) & HIGHS)

char *stpcpy(char *restrict s1, const char *restrict s2)
{
	typedef size_t __attribute__((__may_alias__)) word;
	word *ws1;
	const word *ws2;
	if ((uintptr_t)s2 % ALIGN == (uintptr_t)s1 % ALIGN) {
		for (; (uintptr_t)s2 % ALIGN; s2++, s1++)
			if (!(*s1 = *s2))
				return s1;
		ws1 = (void *)s1;
		ws2 = (const void *)s2;
		for (; !HASZERO(*ws2); *ws1++ = *ws2++)
			;
		s1 = (void *)ws1;
		s2 = (const void *)ws2;
	}

	for (; (*s1 = *s2); s2++, s1++)
		;
	return s1;
}
