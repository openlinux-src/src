#ifndef __ICONV_H
#define __ICONV_H

typedef void *iconv_t;
typedef __SIZE_TYPE__ size_t;

size_t iconv(iconv_t, char **restrict, size_t *restrict, char **restrict,
	     size_t *restrict);
int iconv_close(iconv_t);
iconv_t iconv_open(const char *, const char *);

#endif
