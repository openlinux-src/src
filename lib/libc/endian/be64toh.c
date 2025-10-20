#include <endian.h>

uint64_t be64toh(uint64_t big_endian_64bits)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
	return __builtin_bswap64(big_endian_64bits);
#else
	return big_endian_64bits;
#endif
}
