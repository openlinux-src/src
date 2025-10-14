#include <io.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFSIZ 4096

struct __FILE __stderr = { .fd = STDERR_FILENO,
			   .flags = O_WRONLY,
			   .type = 0,
			   .buf = 0,
			   .buf_len = 0,
			   .buf_size = 0,
			   .buf_pos = 0,
			   .eof = 0,
			   .unget_cnt = 0,
			   .next = 0,
			   .offset = 0,
			   .lock = ATOMIC_FLAG_INIT };

struct FILE *const stderr = (struct FILE *)&__stderr;
