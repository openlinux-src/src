#include <io.h>
#include <fcntl.h>
#include <atomic.h>
#include <unistd.h>

#define BUFSIZ 4096

static atomic_flag __stdio_lock = ATOMIC_FLAG_INIT;
static char __stdout_buffer[BUFSIZ];
struct __FILE __stdout = { .fd = STDOUT_FILENO,
			   .flags = O_WRONLY,
			   .type = 1, // _IOLBF
			   .buf = __stdout_buffer,
			   .buf_len = 0,
			   .buf_size = sizeof(__stdout_buffer),
			   .buf_pos = 0,
			   .eof = 0,
			   .unget_cnt = 0,
			   .offset = 0,
			   .next = NULL,
			   .lock = ATOMIC_FLAG_INIT };

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
