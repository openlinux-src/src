#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>

int vsnprintf(char *restrict s, size_t n, const char *restrict format,
	      va_list ap)
{
	int r;
	FILE f;

	if (n == 0) {
		return 0;
	}

	if (!s) {
		return -1;
	}

	memset(&f, 0, sizeof(f));
	f.fd = -1;
	f.flags = O_WRONLY;
	f.type = _IONBF;
	atomic_flag_clear(&f.lock);
	f.buf = s;
	f.buf_size = n;
	f.buf_len = 0;
	f.buf_pos = 0;
	f.eof = 0;
	f.unget_cnt = 0;
	f.offset = 0;
	f.next = NULL;

	r = vfprintf(&f, format, ap);

	if (f.buf_len < n) {
		s[f.buf_len] = '\0';
	} else if (n > 0) {
		s[n - 1] = '\0';
		r = n - 1;
	}

	return r;
}
