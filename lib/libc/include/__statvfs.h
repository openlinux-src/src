#ifndef __LIBC_STATVFS_H__
#define __LIBC_STATVFS_H__

typedef __UINT64_TYPE__ fsblkcnt_t;
typedef __UINT64_TYPE__ fsfilcnt_t;

struct __statvfs {
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

#endif
