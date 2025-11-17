#include <stdio.h>

void backtrace(void)
{
	void **rbp;
#if defined(__x86_64__)
	__asm__("movq %%rbp, %0" : "=r"(rbp));
#elif defined(__aarch64__)
	__asm__("mov %0, x29" : "=r"(rbp));
#else
#error "Unsupported arch"
#endif

	for (int i = 0; i < 32 && rbp; i++) {
		void *ret = rbp[1];
		if (ret == NULL)
			break;
		printf("  #%02d: %p\n", i, ret);
		rbp = rbp[0];
	}
}
