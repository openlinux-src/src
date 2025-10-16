#include <libc.h>
#include <stdio.h>

char *fgets(char *restrict s, int n, FILE *restrict stream)
{
	return fread(s, 1, n, stream) ? s : NULL;
}

weak_reference(fgets, fgets_unlocked);
