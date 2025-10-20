// https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/arpa_inet.h.html

#include "core.h"

#include <arpa/inet.h>

TEST(arpa_inet_h)
{
	TYPE(in_port_t);
	TYPE(in_addr_t);

	TYPE(struct in_addr);

	MACRO(INET_ADDRSTRLEN);
	MACRO(INET6_ADDRSTRLEN);

	FUNCTION(htonl, FN(uint32_t, uint32_t))
	FUNCTION(htons, FN(uint16_t, uint16_t))
	FUNCTION(ntohl, FN(uint32_t, uint32_t))
	FUNCTION(ntohs, FN(uint16_t, uint16_t))

	TYPE(uint32_t);
	TYPE(uint16_t);

	FUNCTION(inet_addr, FN(in_addr_t, const char *))
	FUNCTION(inet_ntoa, FN(char *, struct in_addr))
	FUNCTION(inet_ntop, FN(const char *, int, const void *restrict,
			       char *restrict, socklen_t));
	FUNCTION(inet_pton, FN(int, int, const char *restrict, void *restrict))
}
