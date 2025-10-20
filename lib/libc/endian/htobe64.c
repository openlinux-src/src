#include <endian.h>

uint64_t htobe64(uint64_t host_64bits)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
	return __builtin_bswap64(host_64bits);
#else
	return host_64bits;
#endif
}
