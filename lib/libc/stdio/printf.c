#include <stdio.h>
#include <stdarg.h>

int printf(const char *restrict format, ...)
{
	int r;
	va_list ap;
	va_start(ap, format);
	r = vfprintf(stdout, format, ap);
	va_end(ap);
	return r;
}
