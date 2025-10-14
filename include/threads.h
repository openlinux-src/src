#ifndef __THREADS_H
#define __THREADS_H

#define thread_local _Thread_local
typedef struct __thread_self *thrd_t;

thrd_t thrd_current(void);

#endif
