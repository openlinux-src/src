#ifndef __DLFCN_H
#define __DLFCN_H

typedef struct {
	const char *dli_fname;
	void *dli_fbase;
	const char *dli_sname;
	void *dli_saddr;
} Dl_info_t;

#define RTLD_LOCAL  0x00000
#define RTLD_LAZY   0x00001
#define RTLD_NOW    0x00002
#define RTLD_GLOBAL 0x00100

int dladdr(const void *restrict, Dl_info_t *restrict);
int dlclose(void *);
char *dlerror(void);
void *dlopen(const char *, int);
void *dlsym(void *restrict, const char *restrict);

#endif
