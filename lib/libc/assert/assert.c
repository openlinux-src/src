#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

_Noreturn void __assert(const char *_Nonnull __file, int __line,
			const char *_Nonnull __func,
			const char *_Nonnull __expr)
{
	fprintf(stderr, "%s:%d: %s: assertion \"%s\" failed\n", __file, __line,
		__func, __expr);
	abort();
}
