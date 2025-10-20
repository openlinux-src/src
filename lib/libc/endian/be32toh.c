#include <endian.h>

uint32_t be32toh(uint32_t big_endian_32bits)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
	return __builtin_bswap32(big_endian_32bits);
#else
	return big_endian_32bits;
#endif
}
