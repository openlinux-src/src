#include <io.h>
#include <stdio.h>

int vdprintf(int fildes, const char *restrict format, va_list ap)
{
	int r;
	FILE f;
	f.fd = fildes;
	f.type = _IONBF;
	r = vfprintf(&f, format, ap);
	return r;
}
