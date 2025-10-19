#ifndef __LIBC_ATOMIC_H
#define __LIBC_ATOMIC_H

#include <stdatomic.h>

#define LIBC_LOCK(__lock)   __libc_lock(&__lock)
#define LIBC_UNLOCK(__lock) atomic_flag_clear(&__lock)

static __inline void __libc_lock(volatile atomic_flag *lock)
{
	while (atomic_flag_test_and_set_explicit(lock, memory_order_acquire)) {
		unsigned int spins = 1;
		do {
			for (unsigned int i = 0; i < spins; i++)
				__asm__ volatile("pause");
			if (spins < 64)
				spins *= 2;
		} while (atomic_flag_test_and_set_explicit(
			lock, memory_order_relaxed));
	}
}

#endif
