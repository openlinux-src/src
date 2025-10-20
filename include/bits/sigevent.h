#ifndef __BITS_SIGEVENT_H
#define __BITS_SIGEVENT_H

#ifndef __BITS_SIGEVENT_H_
#error "Internal header — include the public API header instead."
#endif

union sigval {
	int sival_int;
	void *sival_ptr;
};

struct sigevent {
	int sigev_notify;
	int sigev_signo;
	union sigval sigev_value;
	void (*sigev_notify_function)(union sigval);
};

#endif
