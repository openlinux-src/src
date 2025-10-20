#ifndef __NETINET_IN_H
#define __NETINET_IN_H

#define __BITS_IN_ADDR_H_
#include <bits/in_addr.h>
#undef __BITS_IN_ADDR_H_

#define IN6ADDR_ANY_INIT                            \
	{                                           \
		{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}} \
	}
#define IN6ADDR_LOOPBACK_INIT                       \
	{                                           \
		{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}} \
	}

#define IPPROTO_IP   0
#define IPPROTO_IPV6 41
#define IPPROTO_ICMP 1
#define IPPROTO_RAW  255
#define IPPROTO_TCP  6
#define IPPROTO_UDP  17

#define INADDR_ANY	 ((unsigned long int)0x00000000)
#define INADDR_BROADCAST ((unsigned long int)0xffffffff)

#define INET_ADDRSTRLEN	 16
#define INET6_ADDRSTRLEN 46

#define IPV6_JOIN_GROUP	    20
#define IPV6_LEAVE_GROUP    21
#define IPV6_MULTICAST_HOPS 18
#define IPV6_MULTICAST_IF   17
#define IPV6_UNICAST_HOPS   16

#define IN6_IS_ADDR_UNSPECIFIED(a)                                   \
	((((a)->s6_addr32[0]) == 0) && (((a)->s6_addr32[1]) == 0) && \
	 (((a)->s6_addr32[2]) == 0) && (((a)->s6_addr32[3]) == 0))

#define IN6_IS_ADDR_LOOPBACK(a)                                      \
	((((a)->s6_addr32[0]) == 0) && (((a)->s6_addr32[1]) == 0) && \
	 (((a)->s6_addr32[2]) == 0) && (((a)->s6_addr32[3]) == ntohl(1)))

#define IN6_IS_ADDR_MULTICAST(a) (((uint8_t *)(a))[0] == 0xff)

#define IN6_IS_ADDR_LINKLOCAL(a) \
	((((uint8_t *)(a))[0]) == 0xfe && (((uint8_t *)(a))[1] & 0xc0) == 0x80)

#define IN6_IS_ADDR_SITELOCAL(a) \
	((((uint8_t *)(a))[0]) == 0xfe && (((uint8_t *)(a))[1] & 0xc0) == 0xc0)

#define IN6_IS_ADDR_V4MAPPED(a)                                      \
	((((a)->s6_addr32[0]) == 0) && (((a)->s6_addr32[1]) == 0) && \
	 (((a)->s6_addr32[2]) == ntohl(0x0000ffff)))

#define IN6_IS_ADDR_V4COMPAT(a)                                      \
	((((a)->s6_addr32[0]) == 0) && (((a)->s6_addr32[1]) == 0) && \
	 (((a)->s6_addr32[2]) == 0) && (((a)->s6_addr32[3]) != 0) && \
	 (((a)->s6_addr32[3]) != ntohl(1)))

#define IN6_IS_ADDR_MC_NODELOCAL(a) \
	(IN6_IS_ADDR_MULTICAST(a) && ((((uint8_t *)(a))[1] & 0xf) == 0x1))

#define IN6_IS_ADDR_MC_LINKLOCAL(a) \
	(IN6_IS_ADDR_MULTICAST(a) && ((((uint8_t *)(a))[1] & 0xf) == 0x2))

#define IN6_IS_ADDR_MC_SITELOCAL(a) \
	(IN6_IS_ADDR_MULTICAST(a) && ((((uint8_t *)(a))[1] & 0xf) == 0x5))

#define IN6_IS_ADDR_MC_ORGLOCAL(a) \
	(IN6_IS_ADDR_MULTICAST(a) && ((((uint8_t *)(a))[1] & 0xf) == 0x8))

#define IN6_IS_ADDR_MC_GLOBAL(a) \
	(IN6_IS_ADDR_MULTICAST(a) && ((((uint8_t *)(a))[1] & 0xf) == 0xe))

typedef __UINT8_TYPE__ uint8_t;
typedef __UINT16_TYPE__ in_port_t;
typedef __UINT16_TYPE__ sa_family_t;
typedef __UINT32_TYPE__ uint32_t;

struct sockaddr_in {
	sa_family_t sin_family;
	in_port_t sin_port;
	struct in_addr sin_addr;
};

struct in6_addr {
	uint8_t s6_addr[16];
};

struct sockaddr_in6 {
	sa_family_t sin6_family;
	in_port_t sin6_port;
	uint32_t sin6_flowinfo;
	struct in6_addr sin6_addr;
	uint32_t sin6_scope_id;
};

struct ipv6_mreq {
	struct in6_addr ipv6mr_multiaddr;
	unsigned ipv6mr_interface;
};

const struct in6_addr in6addr_any;
const struct in6_addr in6addr_loopback;

#endif
