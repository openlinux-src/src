#include <io.h>
#include <stdarg.h>
#include <stdio.h>

// TODO: maybe use memstream in future??
int vasprintf(char **restrict ptr, const char *restrict format, va_list ap)
{
	int l;
	va_list ap2;
	va_copy(ap2, ap);
	l = vsnprintf(0, 0, format, ap2);
	va_end(ap2);

	if (l < 0 || !(*ptr = malloc(l + 1U)))
		return -1;

	return vsnprintf(*ptr, l + 1U, format, ap);
}
