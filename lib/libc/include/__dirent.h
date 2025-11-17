#ifndef __LIBC_DIRENT_H
#define __LIBC_DIRENT_H

#include <libc.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>

struct linux_dirent64 {
	uint64_t d_ino;
	int64_t d_off;
	unsigned short d_reclen;
	unsigned char d_type;
	char d_name[];
};

struct __DIR {
	int fildes;
	int cached;
	off_t tell;
	off_t offset;
	char buffer[BUFSIZ] __attribute__((__aligned__(8)));
};

#endif
