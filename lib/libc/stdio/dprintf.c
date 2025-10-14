#include <stdio.h>
#include <stdarg.h>

int dprintf(int fildes, const char *restrict format, ...)
{
	int r;
	va_list ap;
	va_start(ap, format);
	r = vdprintf(fildes, format, ap);
	va_end(ap);
	return r;
}
