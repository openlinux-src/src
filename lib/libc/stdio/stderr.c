#include <io.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFSIZ 4096
#define _IOLBF 0x1

char __stderr_buffer[BUFSIZ];
struct __FILE __stderr = { .fd = STDOUT_FILENO,
			   .flags = O_WRONLY,
			   .buf = __stderr_buffer,
			   .type = 0x0,
			   .buf_size = sizeof(__stderr_buffer),
			   .buf_pos = 0,
			   .offset = 0 };

struct FILE *const stderr = (struct FILE *)&__stderr;
