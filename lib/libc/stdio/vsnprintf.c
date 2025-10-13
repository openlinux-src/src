#include <io.h>
#include <stdio.h>

int vsnprintf(char *restrict s, size_t n, const char *restrict format,
	      va_list ap)
{
	return __vsnprintf((char **)&s, n, format, ap);
}
