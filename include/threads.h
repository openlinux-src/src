#ifndef __THREADS_H
#define __THREADS_H

#define thread_local _Thread_local
#define ONCE_FLAG_INIT
#define TSS_DTOR_ITERATIONS

#define __BITS_TIMESPEC_H_
#include <bits/timespec.h>
#undef __BITS_TIMESPEC_H_

typedef struct __thread_cond_t cnd_t;
typedef struct __thread_mutex_t mtx_t;
typedef struct __thread_once_flag once_flag;
typedef struct __thread_self *thrd_t;
typedef struct __thread_key tss_t;

typedef int (*thrd_start_t)(void *);
typedef void (*tss_dtor_t)(void *);

enum {
	mtx_plain = 0x1,
	mtx_recursive = 0x2,
	mtx_timed = 0x4,
};

enum {
	thrd_success = 0,
	thrd_busy = 1,
	thrd_error = 2,
	thrd_nomem = 3,
	thrd_timedout = 4,
};

void call_once(once_flag *, void (*)(void));
int cnd_broadcast(cnd_t *);
void cnd_destroy(cnd_t *);
int cnd_init(cnd_t *);
int cnd_signal(cnd_t *);
int cnd_timedwait(cnd_t *restrict, mtx_t *restrict,
		  const struct timespec *restrict);
int cnd_wait(cnd_t *, mtx_t *);
void mtx_destroy(mtx_t *);
int mtx_init(mtx_t *, int);
int mtx_lock(mtx_t *);
int mtx_timedlock(mtx_t *restrict, const struct timespec *restrict);
int mtx_trylock(mtx_t *);
int mtx_unlock(mtx_t *);
int thrd_create(thrd_t *, thrd_start_t, void *);
thrd_t thrd_current(void);
int thrd_detach(thrd_t);
int thrd_equal(thrd_t, thrd_t);
_Noreturn void thrd_exit(int);
int thrd_join(thrd_t, int *);
int thrd_sleep(const struct timespec *, struct timespec *);
void thrd_yield(void);
int tss_create(tss_t *, tss_dtor_t);
void tss_delete(tss_t);
void *tss_get(tss_t);
int tss_set(tss_t, void *);

#endif
