#include <linux/auxvec.h>
#include <linux/elf.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <thread.h>
#include <unistd.h>

#define weak_reference(old, new) \
	extern __typeof(old) new __attribute__((__weak__, __alias__(#old)))

extern int main(int, char *[]);
char **environ;

static struct __thread_self thread = { .tid = 0, ._errno = 0 };

struct __attribute__((packed)) auxv_t {
	uintptr_t a_type;
	uintptr_t a_val;
} *__auxv;

static void __vdso_setup(Elf64_Ehdr *vdso_addr)
{
}

weak_reference(__vdso_setup, vdso_setup);

__attribute__((used)) void __libc_start(uintptr_t *sp)
{
	char **argv;
	int argc;

	argc = (int)(*sp);
	argv = (char **)(++sp);
	sp += argc;
	environ = (char **)(++sp);

	while (*sp)
		sp++;

	__auxv = (struct auxv_t *)(++sp);
	while (__auxv->a_type != AT_NULL) {
		if (__auxv->a_type == AT_SYSINFO_EHDR) {
			vdso_setup((Elf64_Ehdr *)__auxv->a_val);
		}

		__auxv++;
	}

	__asm__ volatile("wrfsbase %0" ::"r"(thread));

	exit(main(argc, argv));
}

__attribute__((noreturn, naked)) void _start(void)
{
	__asm__ __volatile__("mov %rsp, %rdi\n"
			     "call __libc_start\n");
}
