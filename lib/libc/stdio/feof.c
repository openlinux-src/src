#include <stdio.h>
#include <io.h>

int feof(FILE *stream)
{
	if (!stream)
		return 0;

	return stream->eof;
}
