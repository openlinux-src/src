#ifndef __LIBC_IO
#define __LIBC_IO

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <stdatomic.h>

typedef __SIZE_TYPE__ size_t;

#define _IO_ERR 0x4
#define _IO_EOF 0x8

struct __FILE {
	int fd;
	uint32_t flags;
	int type;
	atomic_flag lock;
	char *buf;
	int eof;
	size_t buf_size;
	size_t buf_pos;
	size_t buf_len;
	unsigned char unget_buf[16];
	size_t unget_cnt;
	off_t offset;
	struct __FILE *next;
};

void __libc_fadd(struct __FILE *f);

#endif
