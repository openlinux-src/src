#include <string.h>

size_t strlcpy(char *restrict dst, const char *restrict src, size_t dstsize)
{
	size_t srclen = strlen(src);

	if (dstsize == 0) {
		return srclen;
	}

	size_t copylen = srclen < dstsize - 1 ? srclen : dstsize - 1;
	memcpy(dst, src, copylen);
	dst[copylen] = '\0';

	return srclen;
}
