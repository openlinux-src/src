#include <endian.h>

uint16_t le16toh(uint16_t little_endian_16bits)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
	return little_endian_16bits;
#else
	return __builtin_bswap16(little_endian_16bits);
#endif
}
