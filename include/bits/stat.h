#ifndef __BITS_STAT_H
#define __BITS_STAT_H

#ifndef __BITS_STAT_H_
#error "Internal header — include the public API header instead."
#endif

#define __BITS_TIMESPEC_H_
#include <bits/timespec.h>
#undef __BITS_TIMESPEC_H_

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

#endif
