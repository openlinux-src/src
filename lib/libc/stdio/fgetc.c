#include <stdio.h>

int fgetc(FILE *stream)
{
	int c;

	if (fread(&c, 1, 1, stream) < 0)
		return -1;

	return c;
}
