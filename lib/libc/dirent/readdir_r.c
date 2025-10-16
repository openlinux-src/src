#include <__dirent.h>
#include <stddef.h>
#include <errno.h>
#include <dirent.h>
#include <stdint.h>
#include <limits.h>
#include <syscall.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

struct linux_dirent64 {
	uint64_t d_ino;
	int64_t d_off;
	unsigned short d_reclen;
	unsigned char d_type;
	char d_name[];
};

int readdir_r(DIR *restrict dirp, struct dirent *restrict entry,
	      struct dirent **restrict result)
{
	int fildes, ret;
	char buf[sizeof(struct linux_dirent64) + NAME_MAX + 1]
		__attribute__((aligned(__alignof__(struct linux_dirent64))));
	struct linux_dirent64 *ldir = (void *)buf;
	intptr_t i = (intptr_t)dirp;

	if (i >= 0) {
		return EBADF;
	}

	fildes = ~i;

	ret = syscall(getdents64, fildes, ldir, sizeof(buf));
	if (ret < 0) {
		return errno;
	}

	if (ret == 0) {
		*result = NULL;
		return 0;
	}

	ret = lseek(fildes, ldir->d_off, SEEK_SET);
	if (ret < 0)
		return ret;

	entry->d_ino = ldir->d_ino;
	strlcpy(entry->d_name, ldir->d_name, sizeof(entry->d_name));
	*result = entry;
	return 0;
}
