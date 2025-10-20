// https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/aio.h.html

#include "core.h"

#include <aio.h>

TEST(aio_h)
{
	MACRO_VALUE(AIO_CANCELED, 0);
	MACRO_VALUE(AIO_NOTCANCELED, 1);
	MACRO_VALUE(AIO_ALLDONE, 2);

	MACRO_VALUE(LIO_READ, 0);
	MACRO_VALUE(LIO_WRITE, 1);
	MACRO_VALUE(LIO_NOP, 2);

	MACRO_VALUE(LIO_WAIT, 0);
	MACRO_VALUE(LIO_NOWAIT, 1);

	TYPE(off_t);
	TYPE(size_t);
	TYPE(ssize_t);
	TYPE(struct aiocb);
	TYPE(struct sigevent);
	TYPE(struct timespec);

	FUNCTION(aio_cancel, int (*_)(int, struct aiocb *));
	FUNCTION(aio_error, int (*_)(const struct aiocb *));
	FUNCTION(aio_fsync, int (*_)(int, struct aiocb *));
	FUNCTION(aio_read, int (*_)(struct aiocb *));
	FUNCTION(aio_return, ssize_t(*_)(struct aiocb *));
	FUNCTION(aio_suspend, int (*_)(const struct aiocb *const[], int,
				       const struct timespec *));
	FUNCTION(lio_listio, int (*_)(int, struct aiocb *restrict const[], int,
				      struct sigevent *restrict));
}
