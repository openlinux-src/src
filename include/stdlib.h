#ifndef __STDLIB_H__
#define __STDLIB_H__

#ifndef NULL
#define NULL ((void *)0)
#endif

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define WNOHANG 1
#define WUNTRACED 2

#define WEXITSTATUS(s) (((s) & 0xff00) >> 8)
#define WTERMSIG(s) ((s) & 0x7f)
#define WSTOPSIG(s) WEXITSTATUS(s)
#define WIFEXITED(s) (!WTERMSIG(s))
#define WIFSTOPPED(s) ((short)((((s) & 0xffff) * 0x10001U) >> 8) > 0x7f00)
#define WIFSIGNALED(s) (((s) & 0xffff) - 1U < 0xffu)

typedef __SIZE_TYPE__ size_t;

_Noreturn void _Exit(int);
_Noreturn void exit(int);

void *malloc(size_t);
void *realloc(void *, size_t);
void *calloc(size_t, size_t);

long strtol(const char *restrict, char **restrict, int);
long long strtoll(const char *restrict, char **restrict, int);
unsigned long strtoul(const char *restrict, char **restrict, int);
unsigned long long strtoull(const char *restrict, char **restrict, int);
float strtof(const char *restrict, char **restrict);
double strtod(const char *restrict, char **restrict);
int atoi(const char *);
double atof(const char *);
long atol(const char *);
long long atoll(const char *);
long double strtold(const char *restrict, char **restrict);
void qsort(void *, size_t, size_t, int (*)(const void *, const void *));
void qsort_r(void *, size_t, size_t,
	     int (*)(const void *, const void *, void *), void *);
void free(void *);

#endif
