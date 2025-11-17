#include <setjmp.h>

__attribute__((naked, returns_twice)) int setjmp(jmp_buf env)
{
	__asm__ __volatile__("mov %rbx,(%rdi)\n"
			     "mov %rbp,8(%rdi)\n"
			     "mov %r12,16(%rdi)\n"
			     "mov %r13,24(%rdi)\n"
			     "mov %r14,32(%rdi)\n"
			     "mov %r15,40(%rdi)\n"
			     "lea 8(%rsp),%rdx\n"
			     "mov %rdx,48(%rdi)\n"
			     "mov (%rsp),%rdx\n"
			     "mov %rdx,56(%rdi)\n"
			     "xor %eax,%eax\n"
			     "ret\n");
}
