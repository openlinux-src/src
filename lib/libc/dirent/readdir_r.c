/* Maintainer: <contact@bellrise.net> */

#include <__dirent.h>
#include <errno.h>
#include <dirent.h>
#include <syscall.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stddef.h>

#include <stdio.h>

int readdir_r(DIR *restrict dirp, struct dirent *restrict entry,
	      struct dirent **restrict result)
{
	struct linux_dirent64 *ldir = (void *)dirp->buffer;
	ssize_t nread;
	int ret;

	if (dirp == NULL || entry == NULL || result == NULL) {
		return EINVAL;
	}

	/* Clear the result entry to prevent garbage data */
	memset(entry, 0, sizeof(*entry));

	if (dirp->cached) {
		ldir = (void *)(dirp->buffer + dirp->offset);

		/* Validate buffer bounds */
		if (dirp->offset >= sizeof(dirp->buffer) ||
		    dirp->offset + sizeof(struct linux_dirent64) >
			    sizeof(dirp->buffer)) {
			dirp->cached = 0;
			*result = NULL;
			return 0;
		}

		/* Validate record length */
		if (ldir->d_reclen <
			    offsetof(struct linux_dirent64, d_name) + 1 ||
		    dirp->offset + ldir->d_reclen > sizeof(dirp->buffer) ||
		    ldir->d_reclen == 0) {
			dirp->cached = 0;
			*result = NULL;
			return 0;
		}

		entry->d_ino = ldir->d_ino;

		/* Calculate available space for name */
		size_t max_name_len = ldir->d_reclen -
				      offsetof(struct linux_dirent64, d_name);
		if (max_name_len > sizeof(entry->d_name) - 1) {
			max_name_len = sizeof(entry->d_name) - 1;
		}

		/* Find actual string length, bounded by available space */
		size_t name_len = 0;
		while (name_len < max_name_len &&
		       ldir->d_name[name_len] != '\0') {
			name_len++;
		}

		memcpy(entry->d_name, ldir->d_name, name_len);
		entry->d_name[name_len] = '\0';

		dirp->cached--;
		dirp->offset += ldir->d_reclen;
		*result = entry;
		return 0;
	}

	/* Clear cached entries and reset offset */
	dirp->cached = 0;
	dirp->offset = 0;

	ret = syscall(getdents64, dirp->fildes, dirp->buffer,
		      sizeof(dirp->buffer));
	if (ret < 0)
		return errno;

	if (ret == 0) {
		*result = NULL;
		return 0;
	}

	/* Read first entry from buffer. */
	nread = ret;

	/* Validate first entry bounds */
	if (nread < sizeof(struct linux_dirent64) ||
	    ldir->d_reclen < offsetof(struct linux_dirent64, d_name) + 1 ||
	    ldir->d_reclen > nread || ldir->d_reclen == 0) {
		*result = NULL;
		return EINVAL;
	}

	dirp->offset = ldir->d_reclen;
	entry->d_ino = ldir->d_ino;

	/* Calculate available space for name */
	size_t max_name_len =
		ldir->d_reclen - offsetof(struct linux_dirent64, d_name);
	if (max_name_len > sizeof(entry->d_name) - 1) {
		max_name_len = sizeof(entry->d_name) - 1;
	}

	/* Find actual string length, bounded by available space */
	size_t name_len = 0;
	while (name_len < max_name_len && ldir->d_name[name_len] != '\0') {
		name_len++;
	}

	memcpy(entry->d_name, ldir->d_name, name_len);
	entry->d_name[name_len] = '\0';
	*result = entry;

	/* Count the amount of remaining entries we have cached from getdents.  */
	for (ssize_t buffer_offset = ldir->d_reclen; buffer_offset < nread;) {
		struct linux_dirent64 *next_ldir =
			(void *)(dirp->buffer + buffer_offset);

		/* Validate entry bounds to prevent infinite loops */
		if (buffer_offset + sizeof(struct linux_dirent64) > nread ||
		    next_ldir->d_reclen <
			    offsetof(struct linux_dirent64, d_name) + 1 ||
		    buffer_offset + next_ldir->d_reclen > nread ||
		    next_ldir->d_reclen == 0) {
			break;
		}

		buffer_offset += next_ldir->d_reclen;
		dirp->cached++;
	}

	return 0;
}
