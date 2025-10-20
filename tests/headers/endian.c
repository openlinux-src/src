// https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/endian.h.html

#include "core.h"

#include <endian.h>

TEST(endian_h)
{
	MACRO(BYTE_ORDER);
	MACRO(LITTLE_ENDIAN);
	MACRO(BIG_ENDIAN);

	FUNCTION(be16toh, FN(uint16_t, uint16_t));
	FUNCTION(be32toh, FN(uint32_t, uint32_t));
	FUNCTION(be64toh, FN(uint64_t, uint64_t));

	FUNCTION(htobe16, FN(uint16_t, uint16_t));
	FUNCTION(htobe32, FN(uint32_t, uint32_t));
	FUNCTION(htobe64, FN(uint64_t, uint64_t));

	FUNCTION(htole16, FN(uint16_t, uint16_t));
	FUNCTION(htole32, FN(uint32_t, uint32_t));
	FUNCTION(htole64, FN(uint64_t, uint64_t));

	FUNCTION(le16toh, FN(uint16_t, uint16_t));
	FUNCTION(le32toh, FN(uint32_t, uint32_t));
	FUNCTION(le64toh, FN(uint64_t, uint64_t));

	TYPE(uint16_t);
	TYPE(uint32_t);
	TYPE(uint64_t);
}
