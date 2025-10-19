#ifndef __ARPA_INET_H
#define __ARPA_INET_H

#define INET_ADDRSTRLEN	 16
#define INET6_ADDRSTRLEN 46

typedef __UINT16_TYPE__ in_port_t;
typedef __UINT32_TYPE__ in_addr_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __UINT16_TYPE__ uint16_t;

typedef unsigned socklen_t;

struct in_addr {
	in_addr_t s_addr;
};

uint32_t htonl(uint32_t);
uint16_t htons(uint16_t);
uint32_t ntohl(uint32_t);
uint16_t ntohs(uint16_t);

in_addr_t inet_addr(const char *);
char *inet_ntoa(struct in_addr);
const char *inet_ntop(int, const void *restrict, char *restrict, socklen_t);
int inet_pton(int, const char *restrict, void *restrict);

#endif
