#include <io.h>
#include <fcntl.h>
#include <atomic.h>
#include <unistd.h>

#define BUFSIZ 4096

static atomic_flag __stdio_lock = ATOMIC_FLAG_INIT;
static char __stdout_buffer[BUFSIZ];
struct __FILE __stdout = { .fd = STDOUT_FILENO,
			   .flags = O_WRONLY,
			   .buf = __stdout_buffer,
			   .type = 0x1,
			   .buf_size = BUFSIZ,
			   .buf_pos = 0,
			   .offset = 0,
			   .next = NULL };

struct __FILE *const stdout = (struct __FILE *)&__stdout;

void __libc_fadd(struct __FILE *f)
{
	LIBC_LOCK(__stdio_lock);
	struct __FILE *cur = &__stdout;
	while (cur->next)
		cur = cur->next;
	cur->next = f;
	LIBC_UNLOCK(__stdio_lock);
}
