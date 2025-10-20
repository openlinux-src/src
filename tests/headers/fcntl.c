// https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/fcntl.h.html

#include "core.h"

#include <fcntl.h>

TEST(fcntl_h)
{
	TYPE(mode_t);
	TYPE(off_t);
	TYPE(pid_t);

	TYPE(struct f_owner_ex);
	STRUCT_MEMBER(struct f_owner_ex, int, type);
	STRUCT_MEMBER(struct f_owner_ex, pid_t, pid);

	TYPE(struct flock);
	STRUCT_MEMBER(struct flock, short, l_type);
	STRUCT_MEMBER(struct flock, short, l_whence);
	STRUCT_MEMBER(struct flock, off_t, l_start);
	STRUCT_MEMBER(struct flock, off_t, l_len);
	STRUCT_MEMBER(struct flock, pid_t, l_pid);

	MACRO(F_DUPFD);
	MACRO(F_DUPFD_CLOEXEC);
	MACRO(F_GETFD);
	MACRO(F_SETFD);
	MACRO(F_GETFL);
	MACRO(F_SETFL);
	MACRO(F_GETLK);
	MACRO(F_SETLK);
	MACRO(F_SETLKW);
	MACRO(F_GETOWN);
	MACRO(F_SETOWN);

	MACRO(FD_CLOEXEC);

	MACRO(F_RDLCK);
	MACRO(F_UNLCK);
	MACRO(F_WRLCK);

	MACRO(SEEK_SET);
	MACRO(SEEK_CUR);
	MACRO(SEEK_END);

	MACRO(O_CLOEXEC);
	MACRO(O_CREAT);
	MACRO(O_DIRECTORY);
	MACRO(O_EXCL);
	MACRO(O_NOCTTY);
	MACRO(O_NOFOLLOW);
	MACRO(O_TRUNC);
	MACRO(O_TTY_INIT);

	MACRO(O_APPEND);
	MACRO(O_DSYNC);
	MACRO(O_NONBLOCK);
	MACRO(O_RSYNC);
	MACRO(O_SYNC);

	MACRO(O_ACCMODE);
	MACRO(O_EXEC);
	MACRO(O_RDONLY);
	MACRO(O_RDWR);
	MACRO(O_SEARCH);
	MACRO(O_WRONLY);

	MACRO(AT_FDCWD);
	MACRO(AT_EACCESS);
	MACRO(AT_SYMLINK_NOFOLLOW);
	MACRO(AT_REMOVEDIR);

	MACRO(POSIX_FADV_DONTNEED);
	MACRO(POSIX_FADV_NOREUSE);
	MACRO(POSIX_FADV_NORMAL);
	MACRO(POSIX_FADV_RANDOM);
	MACRO(POSIX_FADV_SEQUENTIAL);
	MACRO(POSIX_FADV_WILLNEED);

	FUNCTION(creat, int (*f)(const char *, mode_t));
	FUNCTION(fcntl, int (*f)(int, int, ...));
	FUNCTION(open, int (*f)(const char *, int, ...));
	FUNCTION(openat, int (*f)(int, const char *, int, ...));
	FUNCTION(posix_fadvise, int (*f)(int, off_t, off_t, int));
	FUNCTION(posix_fallocate, int (*f)(int, off_t, off_t));
}
