#ifndef __LIBC_IO
#define __LIBC_IO

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <stdatomic.h>

typedef __SIZE_TYPE__ size_t;

#define _IO_ERR	 0x4
#define _IO_EOF	 0x8
#define _IO_WIDE 0x10

/* Buffering modes */
#define _IOFBF 0 /* Full buffering */
#define _IOLBF 1 /* Line buffering */
#define _IONBF 2 /* No buffering */

/* File access modes */
#ifndef O_RDONLY
#define O_RDONLY  0
#define O_WRONLY  1
#define O_RDWR	  2
#define O_ACCMODE 3
#endif

/* Atomic operations for threading */
#define LIBC_LOCK(x)                             \
	while (atomic_flag_test_and_set(&(x))) { \
	}
#define LIBC_UNLOCK(x) atomic_flag_clear(&(x))

struct __FILE {
	int fd;
	uint32_t flags;
	int type;
	pid_t pid;
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
