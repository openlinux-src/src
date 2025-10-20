#ifndef __LIBC_LIBC_H
#define __LIBC_LIBC_H

#include <__locale.h>
#include <stdatomic.h>

#define likely(x)     __builtin_expect(!!(x), 1)
#define unlikely(x)   __builtin_expect(!!(x), 0)
#define unused	      __attribute__((unused))
#define aligned(type) __attribute__((aligned(__alignof__(type))))
#define weak	      __attribute__((__weak__))
#define weak_reference(old, new) \
	extern __typeof(old) new __attribute__((__weak__, __alias__(#old)))

static struct {
	enum {
		LIBC_ENVP_TOUCHED = 1 << 0,
	} flags;
	struct {
		volatile atomic_flag abort;
		volatile atomic_flag malloc;
		volatile atomic_flag environ;
	} lock;
	struct __locale_t locale;
} libc = { .lock = { ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT } };

#endif
