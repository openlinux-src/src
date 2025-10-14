#include <stdio.h>
#include <stdarg.h>

int fprintf(FILE *restrict stream, const char *restrict format, ...)
{
	int r;
	va_list ap;
	va_start(ap, format);
	r = vfprintf(stream, format, ap);
	va_end(ap);
	return r;
}
