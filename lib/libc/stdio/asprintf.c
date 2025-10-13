#include <stdio.h>
#include <stdarg.h>

int asprintf(char **restrict ptr, const char *restrict format, ...)
{
	va_list ap;
	va_start(ap, format);
	int ret = vasprintf(ptr, format, ap);
	va_end(ap);
	return ret;
}
