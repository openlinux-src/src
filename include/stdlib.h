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

_Noreturn void exit(int);

void *malloc(size_t);
void *realloc(void *, size_t);
void *calloc(size_t, size_t);

long strtol(const char *restrict, char **restrict, int);
long long strtoll(const char *restrict nptr, char **restrict endptr, int base);
unsigned long strtoul(const char *restrict nptr, char **restrict endptr,
		      int base);
unsigned long long strtoull(const char *restrict nptr, char **restrict endptr,
			    int base);
float strtof(const char *restrict nptr, char **restrict endptr);
double strtod(const char *restrict nptr, char **restrict endptr);
long double strtold(const char *restrict nptr, char **restrict endptr);

void free(void *);

#endif
