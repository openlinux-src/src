#ifndef __STATVFS_H
#define __STATVFS_H

#define ST_RDONLY 1
#define ST_NOSUID 2

typedef __UINT64_TYPE__ fsblkcnt_t;
typedef __UINT64_TYPE__ fsfilcnt_t;

struct statvfs {
	unsigned long f_bsize;
	unsigned long f_frsize;
	fsblkcnt_t f_blocks;
	fsblkcnt_t f_bfree;
	fsblkcnt_t f_bavail;
	fsfilcnt_t f_files;
	fsfilcnt_t f_ffree;
	fsfilcnt_t f_favail;
	unsigned long f_fsid;
	unsigned long f_flag;
	unsigned long f_namemax;
};

int fstatvfs(int, struct statvfs *);
int statvfs(const char *restrict, struct statvfs *restrict);

#endif
