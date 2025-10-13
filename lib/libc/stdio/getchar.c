#include <libc.h>
#include <stdio.h>

int getchar(void)
{
	return fgetc(stdin);
}

weak_reference(getchar, getchar_unlocked);
