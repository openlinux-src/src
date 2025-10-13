#include <libc.h>
#include <stdio.h>

int getc(FILE *stream)
{
	return fgetc(stream);
}

weak_reference(getc, getc_unlocked);
