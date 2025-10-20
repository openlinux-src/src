// https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/dirent.h.html

#include "core.h"

#include <dirent.h>

TEST(dirent_h)
{
	TYPE(ino_t);
	TYPE(size_t);
	TYPE(ssize_t);
	TYPE(reclen_t);

	INCOMPLETE_TYPE(DIR);

	TYPE(struct dirent);
	STRUCT_MEMBER(struct dirent, ino_t, d_ino);
	STRUCT_MEMBER_ARRAY(struct dirent, char, d_name);

	TYPE(struct posix_dent);
	STRUCT_MEMBER(struct posix_dent, ino_t, d_ino);
	STRUCT_MEMBER(struct posix_dent, reclen_t, d_reclen);
	STRUCT_MEMBER(struct posix_dent, unsigned char, d_type);
	STRUCT_MEMBER_ARRAY(struct posix_dent, char, d_name);

	MACRO_VALUE(DT_BLK, 0x6);
	MACRO_VALUE(DT_CHR, 0x2);
	MACRO_VALUE(DT_DIR, 0x4);
	MACRO_VALUE(DT_FIFO, 0x1);
	MACRO_VALUE(DT_LNK, 0xa);
	MACRO_VALUE(DT_REG, 0x8);
	MACRO_VALUE(DT_SOCK, 0xc);
	MACRO_VALUE(DT_UNKNOWN, 0x0);

	MACRO_VALUE(DT_MQ, 0xe);
	MACRO_VALUE(DT_SEM, 0x10);
	MACRO_VALUE(DT_SHM, 0x12);
	MACRO_VALUE(DT_TMO, 0x14);

	FUNCTION(alphasort,
		 FN(int, const struct dirent **, const struct dirent **));
	FUNCTION(closedir, FN(int, DIR *));
	FUNCTION(dirfd, FN(int, DIR *));
	FUNCTION(fdopendir, FN(DIR *, int));
	FUNCTION(opendir, FN(DIR *, const char *));
	FUNCTION(posix_getdents, FN(ssize_t, int, void *, size_t, int));
	FUNCTION(readdir, FN(struct dirent *, DIR *));
	FUNCTION(readdir_r, FN(int, DIR *restrict, struct dirent *restrict,
			       struct dirent **restrict));
	FUNCTION(rewinddir, FN(void, DIR *));
	FUNCTION(scandir,
		 FN(int, const char *, struct dirent ***,
		    int (*)(const struct dirent *),
		    int (*)(const struct dirent **, const struct dirent **)));
	FUNCTION(seekdir, FN(void, DIR *, long));
	FUNCTION(telldir, FN(long, DIR *));
}
