#ifndef __AIO_H
#define __AIO_H

#define __BITS_SIGEVENT_H_
#include <bits/sigevent.h>
#undef __BITS_SIGEVENT_H_

#define __BITS_TIMESPEC_H_
#include <bits/timespec.h>
#undef __BITS_TIMESPEC_H_

/** A return value indicating that all requested operations have been canceled. */
#define AIO_CANCELED 0

/** A return value indicating that some of the requested operations could not be canceled since they are in progress. */
#define AIO_NOTCANCELED 1

/** A return value indicating that none of the requested operations could be canceled since they are already complete. */
#define AIO_ALLDONE 2

#define LIO_READ  0
#define LIO_WRITE 1
#define LIO_NOP	  2

#define LIO_WAIT   0
#define LIO_NOWAIT 1

typedef __INT64_TYPE__ off_t;
typedef __SIZE_TYPE__ size_t;
typedef __INT64_TYPE__ ssize_t;

struct aiocb {
	int aio_fildes;
	off_t aio_offset;
	volatile void *aio_buf;
	size_t aio_nbytes;
	int aio_reqprio;
	struct sigevent aio_sigevent;
	int aio_lio_opcode;
};

int aio_cancel(int, struct aiocb *);
int aio_error(const struct aiocb *);
int aio_fsync(int, struct aiocb *);
int aio_read(struct aiocb *);
ssize_t aio_return(struct aiocb *);
int aio_suspend(const struct aiocb *const[], int, const struct timespec *);
int aio_write(struct aiocb *);
int lio_listio(int, struct aiocb *restrict const[restrict], int,
	       struct sigevent *restrict);

#endif
