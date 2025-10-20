#include <endian.h>

uint64_t le64toh(uint64_t little_endian_64bits)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
	return little_endian_64bits;
#else
	return __builtin_bswap64(little_endian_64bits);
#endif
}
