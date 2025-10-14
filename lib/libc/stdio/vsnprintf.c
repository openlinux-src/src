#include <io.h>
#include <stdio.h>
#include <stdlib.h>

int vsnprintf(char *restrict s, size_t n, const char *restrict format,
	      va_list ap)
{
	int r;
	FILE f;
	f.buf = s;
	f.buf_size = n;
	r = vfprintf(&f, format, ap);
	return r;
}
