#include <__dirent.h>
#include <dirent.h>
#include <stdint.h>

int dirfd(DIR *dirp)
{
	return ~((intptr_t)dirp);
}
