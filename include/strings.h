#ifndef __STRINGS_H
#define __STRINGS_H

typedef __SIZE_TYPE__ size_t;

int ffs(int);
int ffsl(long);
int ffsll(long long);

int strcasecmp(const char *, const char *);
// int strcasecmp_l(const char *, const char *, locale_t);
int strncasecmp(const char *, const char *, size_t);
// int strncasecmp_l(const char *, const char *, size_t, locale_t);

#endif
