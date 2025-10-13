#include <libc.h>
#include <stdio.h>

int putchar(int c)
{
	return putc(c, stdout);
}

weak_reference(putchar, putchar_unlocked);
