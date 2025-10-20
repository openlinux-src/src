#include <endian.h>

uint32_t le32toh(uint32_t little_endian_32bits)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
	return little_endian_32bits;
#else
	return __builtin_bswap32(little_endian_32bits);
#endif
}
