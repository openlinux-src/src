#ifndef __GLOB_H
#define __GLOB_H

#define GLOB_APPEND   0x0001
#define GLOB_DOOFFS   0x0002
#define GLOB_ERR      0x0004
#define GLOB_MARK     0x0008
#define GLOB_NOCHECK  0x0010
#define GLOB_NOESCAPE 0x0020
#define GLOB_NOSORT   0x0040

#define GLOB_ABORTED -1
#define GLOB_NOMATCH -2
#define GLOB_NOSPACE -3

typedef __SIZE_TYPE__ size_t;

typedef struct {
	size_t gl_pathc;
	char **gl_pathv;
	size_t gl_offs;
} glob_t;

int glob(const char *restrict, int, int (*)(const char *, int),
	 glob_t *restrict);
void globfree(glob_t *);

#endif
