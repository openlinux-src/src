#include <setjmp.h>

__attribute__((noreturn, naked, returns_twice)) void longjmp(jmp_buf env,
							     int val)
{
	__asm__ __volatile__("mov 0(%rdi), %rbx\n"
			     "mov 8(%rdi), %rbp\n"
			     "mov 16(%rdi), %r12\n"
			     "mov 24(%rdi), %r13\n"
			     "mov 32(%rdi), %r14\n"
			     "mov 40(%rdi), %r15\n"
			     "mov 48(%rdi), %rsp\n"
			     "xor %rax, %rax\n"
			     "cmp $0, %esi\n"
			     "setne %al\n"
			     "mov %eax, %eax\n"
			     "jmp *56(%rdi)\n");
}
