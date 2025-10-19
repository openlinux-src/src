#ifndef __FCNTL_H
#define __FCNTL_H

#define F_DUPFD		0
#define F_DUPFD_CLOEXEC 02000000
#define F_DUPFD_CLOFORK 0
#define F_GETFD		1
#define F_SETFD		2
#define F_GETFL		3
#define F_SETFL		4
#define F_GETLK		5
#define F_SETLK		6
#define F_SETLKW	7
#define F_OFD_GETLK	36
#define F_OFD_SETLK	37
#define F_OFD_SETLKW	38

#define F_GETOWN    9
#define F_GETOWN_EX 16
#define F_SETOWN    8
#define F_SETOWN_EX 15

#define FD_CLOEXEC 1
#define FD_CLOFORK

#define F_RDLCK 0
#define F_UNLCK 2
#define F_WRLCK 1

#define F_OWNER_PID  1
#define F_OWNER_PGRP 2

#undef SEEK_SET
#undef SEEK_CUR
#undef SEEK_END
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define O_CLOEXEC 02000000
#define O_CLOFORK
#define O_CREAT	    00000100
#define O_DIRECTORY 00200000
#define O_EXCL	    00000200
#define O_NOCTTY    00000400
#define O_NOFOLLOW  00400000
#define O_TRUNC	    00001000
#define O_TTY_INIT  0

#define O_APPEND   00002000
#define O_DSYNC	   00010000
#define O_NONBLOCK 00004000
#define O_SYNC	   (04000000 | O_DSYNC)
#define O_RSYNC

#define O_ACCMODE 00000003
#define O_EXEC	  010000000
#define O_RDONLY  000000000
#define O_RDWR	  00000002
#define O_SEARCH  010000000
#define O_WRONLY  000000001

#define AT_FDCWD	    (-100)
#define AT_EACCESS	    0x200
#define AT_SYMLINK_NOFOLLOW 0x100
#define AT_SYMLINK_FOLLOW   0x400

#define AT_REMOVEDIR	      0x200
#define POSIX_FADV_DONTNEED   5
#define POSIX_FADV_NOREUSE    4
#define POSIX_FADV_NORMAL     0
#define POSIX_FADV_RANDOM     1
#define POSIX_FADV_SEQUENTIAL 2
#define POSIX_FADV_WILLNEED   3

typedef __UINT32_TYPE__ mode_t;
typedef __INT64_TYPE__ off_t;
typedef __INT64_TYPE__ pid_t;

struct f_owner_ex {
	int type;
	pid_t pid;
	short l_type;
	short l_whence;
	off_t l_start;
	off_t l_len;
	pid_t l_pid;
};

int creat(const char *, mode_t);
int fcntl(int, int, ...);
int open(const char *, int, ...);
int openat(int, const char *, int, ...);
int posix_fadvise(int, off_t, off_t, int);
int posix_fallocate(int, off_t, off_t);

#endif
