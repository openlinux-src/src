#include <io.h>

int vasprintf(char **restrict ptr, const char *restrict format, va_list ap)
{
	if (!ptr || !format)
		return -1;

	*ptr = NULL;
	size_t bufsz = 0;

	va_list ap_copy;
	va_copy(ap_copy, ap);

	int ret = __vsnprintf(ptr, bufsz, format, ap_copy);

	va_end(ap_copy);

	if (ret < 0) {
		free(*ptr);
		*ptr = NULL;
		return -1;
	}

	return ret;
}
