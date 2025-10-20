#ifndef __ENDIAN_H
#define __ENDIAN_H

#define BYTE_ORDER    __BYTE_ORDER__
#define LITTLE_ENDIAN __ORDER_LITTLE_ENDIAN__
#define BIG_ENDIAN    __ORDER_BIG_ENDIAN__

typedef __UINT16_TYPE__ uint16_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __UINT64_TYPE__ uint64_t;

uint16_t be16toh(uint16_t);
uint32_t be32toh(uint32_t);
uint64_t be64toh(uint64_t);

uint16_t htobe16(uint16_t);
uint32_t htobe32(uint32_t);
uint64_t htobe64(uint64_t);

uint16_t htole16(uint16_t);
uint32_t htole32(uint32_t);
uint64_t htole64(uint64_t);

uint16_t le16toh(uint16_t);
uint32_t le32toh(uint32_t);
uint64_t le64toh(uint64_t);

#endif
