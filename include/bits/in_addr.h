#ifndef __BITS_IN_ADDR_H
#define __BITS_IN_ADDR_H

#ifndef __BITS_IN_ADDR_H_
#error "Internal header — include the public API header instead."
#endif

typedef __UINT32_TYPE__ in_addr_t;

struct in_addr {
	in_addr_t s_addr;
};

#endif
