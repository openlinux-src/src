#ifndef __SYS_STAT_H
#define __SYS_STAT_H

#include <time.h>

#define S_IFMT 00170000
#define S_IFBLK 0060000
#define S_IFCHR 0020000
#define S_IFIFO 0010000
#define S_IFREG 0100000
#define S_IFDIR 0040000
#define S_IFLNK 0120000
#define S_IFSOCK 0140000

#define S_IRWXU 0000700
#define S_IRUSR 0000400
#define S_IWUSR 0000200
#define S_IXUSR 0000100
#define S_IRWXG 0000070
#define S_IRGRP 0000040
#define S_IWGRP 0000020
#define S_IXGRP 0000010
#define S_IRWXO 0000007
#define S_IROTH 0000004
#define S_IWOTH 0000002
#define S_IXOTH 0000001
#define S_ISUID 0004000
#define S_ISGID 0002000
#define S_ISVTX 0001000

#define S_ISBLK(mode) (((mode) & S_IFMT) == S_IFBLK)
#define S_ISCHR(mode) (((mode) & S_IFMT) == S_IFCHR)
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#define S_ISFIFO(mode) (((mode) & S_IFMT) == S_IFIFO)
#define S_ISREG(mode) (((mode) & S_IFMT) == S_IFREG)
#define S_ISLNK(mode) (((mode) & S_IFMT) == S_IFLNK)
#define S_ISSOCK(mode) (((mode) & S_IFMT) == S_IFSOCK)

#define S_TYPEISMQ(buf) 0
#define S_TYPEISSEM(buf) 0
#define S_TYPEISSHM(buf) 0
#define S_TYPEISTMO(buf) 0

#define UTIME_NOW 0x3fffffff
#define UTIME_OMIT 0x3ffffffe

typedef __UINT64_TYPE__ dev_t;
typedef __UINT64_TYPE__ ino_t;
typedef __UINT32_TYPE__ mode_t;
typedef __UINT32_TYPE__ nlink_t;
typedef __UINT32_TYPE__ uid_t;
typedef __UINT32_TYPE__ gid_t;
typedef __INT64_TYPE__ off_t;
typedef __INT32_TYPE__ blksize_t;
typedef __INT64_TYPE__ blkcnt_t;

struct stat {
	dev_t st_dev;
	ino_t st_ino;
	mode_t st_mode;
	nlink_t st_nlink;
	uid_t st_uid;
	gid_t st_gid;
	dev_t st_rdev;
	off_t st_size;
	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
	blksize_t st_blksize;
	blkcnt_t st_blocks;
};

int chmod(const char *, mode_t);
int fchmod(int, mode_t);
int fchmodat(int, const char *, mode_t, int);
int fstat(int, struct stat *);
int fstatat(int, const char *restrict, struct stat *restrict, int);
int futimens(int, const struct timespec[2]);
int lstat(const char *restrict, struct stat *restrict);
int mkdir(const char *, mode_t);
int mkdirat(int, const char *, mode_t);
int mkfifo(const char *, mode_t);
int mkfifoat(int, const char *, mode_t);
int mknod(const char *, mode_t, dev_t);
int mknodat(int, const char *, mode_t, dev_t);
int stat(const char *restrict, struct stat *restrict);
mode_t umask(mode_t);
int utimensat(int, const char *, const struct timespec[2], int);

#endif
