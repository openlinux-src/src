#include <libc.h>
#include <stdio.h>

int putc(int c, FILE *stream)
{
	return fputc(c, stream);
}

weak_reference(putc, putc_unlocked);
