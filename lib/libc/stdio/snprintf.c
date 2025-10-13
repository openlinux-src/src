#include <io.h>
#include <stdarg.h>
#include <stdio.h>

int snprintf(char *restrict s, size_t n, const char *restrict format, ...)
{
	va_list args;
	va_start(args, format);
	long ret = vsnprintf(s, n, format, args);
	va_end(args);
	return ret;
}
