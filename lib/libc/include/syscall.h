#ifndef __LIBC_SYSCALL_H
#define __LIBC_SYSCALL_H

#include <asm/syscall.h>
#include <asm/unistd_64.h>
#include <errno.h>

typedef long syscall_arg_t;

#define __SYSCALL_NARGS_(a, b, c, d, e, f, g, h, n, ...) n
#define __SYSCALL_NARGS(...) \
	__SYSCALL_NARGS_(__VA_ARGS__, 7, 6, 5, 4, 3, 2, 1, 0, )

#define __SYSCALL_CONCAT_(a, b) a##b
#define __SYSCALL_CONCAT(a, b) __SYSCALL_CONCAT_(a, b)

#define __SYSCALL_(b, ...) \
	__SYSCALL_CONCAT(b, __SYSCALL_NARGS(__VA_ARGS__))(__VA_ARGS__)

#define __syscall(...) __SYSCALL_(__syscall_, __VA_ARGS__)
#define syscall(...) __syscall_ret(__syscall(__VA_ARGS__))

#define __sa(x) ((syscall_arg_t)(x))

#define __syscall_0(n) __syscall0(__NR_##n)
#define __syscall_1(n, a) __syscall1(__NR_##n, __sa(a))
#define __syscall_2(n, a, b) __syscall2(__NR_##n, __sa(a), __sa(b))
#define __syscall_3(n, a, b, c) __syscall3(__NR_##n, __sa(a), __sa(b), __sa(c))
#define __syscall_4(n, a, b, c, d) \
	__syscall4(__NR_##n, __sa(a), __sa(b), __sa(c), __sa(d))
#define __syscall_5(n, a, b, c, d, e) \
	__syscall5(__NR_##n, __sa(a), __sa(b), __sa(c), __sa(d), __sa(e))
#define __syscall_6(n, a, b, c, d, e, f)                                  \
	__syscall6(__NR_##n, __sa(a), __sa(b), __sa(c), __sa(d), __sa(e), \
		   __sa(f))
#define __syscall_7(n, a, b, c, d, e, f, g)                               \
	__syscall7(__NR_##n, __sa(a), __sa(b), __sa(c), __sa(d), __sa(e), \
		   __sa(f), __sa(g))

static __inline long __syscall_ret(long ret)
{
	if (ret > -4096 && ret < 0) {
		errno = -ret;
		ret = -1;
	}
	return ret;
}

#ifdef __NR_pread64
#undef __NR_pread
#define __NR_pread __NR_pread64
#endif

#ifdef __NR_pwrite64
#undef __NR_pwrite
#define __NR_pwrite __NR_pwrite64
#endif

#endif
