#include <stdio.h>
#include <stdarg.h>

int asprintf(char **restrict ptr, const char *restrict format, ...)
{
	int r;
	va_list ap;
	va_start(ap, format);
	r = vasprintf(ptr, format, ap);
	va_end(ap);
	return r;
}
