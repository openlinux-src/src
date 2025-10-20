#ifndef __SYS_UN_H
#define __SYS_UN_H

typedef __UINT16_TYPE__ sa_family_t;

struct sockaddr_un {
	sa_family_t sun_family;
	char sun_path[108];
};

#endif
