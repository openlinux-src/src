// https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/dlfcn.h.html

#include "core.h"

#include <dlfcn.h>

TEST(dlfcn_h)
{
	TYPE(Dl_info_t);

	STRUCT_MEMBER(Dl_info_t, const char *, dli_fname);
	STRUCT_MEMBER(Dl_info_t, void *, dli_fbase);
	STRUCT_MEMBER(Dl_info_t, const char *, dli_sname);
	STRUCT_MEMBER(Dl_info_t, void *, dli_saddr);

	MACRO_VALUE(RTLD_LAZY, 1);
	MACRO_VALUE(RTLD_NOW, 2);
	MACRO_VALUE(RTLD_GLOBAL, 256);
	MACRO_VALUE(RTLD_LOCAL, 0);

	FUNCTION(dladdr, int (*f)(const void *restrict, Dl_info_t *restrict));
	FUNCTION(dlclose, int (*f)(void *));
	FUNCTION(dlerror, char *(*f)(void));
	FUNCTION(dlopen, void *(*f)(const char *, int));
	FUNCTION(dlsym, void *(*f)(void *restrict, const char *restrict));
}
