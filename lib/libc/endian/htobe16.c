#include <endian.h>

uint16_t htobe16(uint16_t host_16bits)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
	return __builtin_bswap16(host_16bits);
#else
	return host_16bits;
#endif
}
