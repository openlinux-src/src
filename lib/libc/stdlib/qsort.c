#include <stdlib.h>

static int wrapper(const void *a, const void *b, void *compar)
{
	return ((int (*)(const void *, const void *))compar)(a, b);
}

void qsort(void *base, size_t nel, size_t width,
	   int (*compar)(const void *, const void *))
{
	qsort_r(base, nel, width, wrapper, compar);
}
