// https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/mqueue.h.html

#include "core.h"

#include <mqueue.h>

TEST(mqueue_h)
{
	TYPE(mqd_t);
	TYPE(size_t);
	TYPE(ssize_t);
	TYPE(struct timespec);

	INCOMPLETE_TYPE(struct sigevent);

	TYPE(struct mq_attr);
	STRUCT_MEMBER(struct mq_attr, long, mq_flags);
	STRUCT_MEMBER(struct mq_attr, long, mq_maxmsg);
	STRUCT_MEMBER(struct mq_attr, long, mq_msgsize);
	STRUCT_MEMBER(struct mq_attr, long, mq_curmsgs);

	FUNCTION(mq_close, FN(int, mqd_t));
	FUNCTION(mq_getattr, FN(int, mqd_t, struct mq_attr *));
	FUNCTION(mq_notify, FN(int, mqd_t, const struct sigevent *));
	FUNCTION(mq_open, FN(mqd_t, const char *, int, ...));
	FUNCTION(mq_receive, FN(ssize_t, mqd_t, char *, size_t, unsigned *));
	FUNCTION(mq_send, FN(int, mqd_t, const char *, size_t, unsigned));
	FUNCTION(mq_setattr, FN(int, mqd_t, const struct mq_attr *restrict,
				struct mq_attr *restrict));
	FUNCTION(mq_timedreceive,
		 FN(ssize_t, mqd_t, char *restrict, size_t, unsigned *restrict,
		    const struct timespec *restrict));
	FUNCTION(mq_timedsend, FN(int, mqd_t, const char *, size_t, unsigned,
				  const struct timespec *));
	FUNCTION(mq_unlink, FN(int, const char *));
}
