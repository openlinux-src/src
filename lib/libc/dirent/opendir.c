#include <__dirent.h>

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

DIR *opendir(const char *dirname)
{
	int fildes;

	fildes = open(dirname, O_RDONLY);
	if (fildes == -1)
		return NULL;

	return fdopendir(fildes);
}
