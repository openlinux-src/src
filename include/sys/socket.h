#ifndef __SYS_SOCKET_H
#define __SYS_SOCKET_H

#define SCM_RIGHTS 0x01

#define CMSG_DATA(cmsg) ((void *)(cmsg) + sizeof(struct cmsghdr))

#define CMSG_NXTHDR(mhdr, cmsg)                                          \
	((cmsg)->cmsg_len < sizeof(struct cmsghdr) ||                    \
			 (((cmsg)->cmsg_len + sizeof(long) - 1) &        \
			  ~(long)(sizeof(long) - 1)) +                   \
					 sizeof(struct cmsghdr) >=       \
				 ((unsigned char *)(mhdr)->msg_control + \
				  (mhdr)->msg_controllen) -              \
					 (unsigned char *)(cmsg) ?       \
		 0 :                                                     \
		 (struct cmsghdr *)((unsigned char *)(cmsg) +            \
				    __CMSG_LEN(cmsg)))

#define CMSG_FIRSTHDR(mhdr)                                         \
	((size_t)(mhdr)->msg_controllen >= sizeof(struct cmsghdr) ? \
		 (struct cmsghdr *)(mhdr)->msg_control :            \
		 (struct cmsghdr *)0)

#define CMSG_SPACE(len) (CMSG_ALIGN(len) + CMSG_ALIGN(sizeof(struct cmsghdr)))

#define CMSG_LEN(len) (CMSG_ALIGN(sizeof(struct cmsghdr)) + (len))

struct linger {
	int l_onoff;
	int l_linger;
};

#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define SOCK_RAW 3
#define SOCK_RDM 4
#define SOCK_SEQPACKET 5
#define SOCK_DCCP 6
#define SOCK_PACKET = 10

// TODO: fill with fcntl values
#define SOCK_NONBLOCK O_NONBLOCK
#define SOCK_CLOEXEC O_CLOEXEC
#define SOCK_CLOFORK 0

#define SO_ACCEPTCONN 30
#define SO_BROADCAST 6
#define SO_DEBUG 1
#define SO_DOMAIN 39
#define SO_DONTROUTE 5
#define SO_ERROR 4
#define SO_KEEPALIVE 9
#define SO_LINGER 13
#define SO_OOBINLINE 10
#define SO_PROTOCOL 38
#define SO_RCVBUF 8
#define SO_RCVLOWAT 18
#define SO_RCVTIMEO 66
#define SO_SNDBUF 7
#define SO_SNDLOWAT 19
#define SO_SNDTIMEO 67
#define SO_TYPEA 3
#define SOL_SOCKET 1

#define SOMAXCONN 4096

#define MSG_CMSG_CLOFORK 0x00000000
#define MSG_OOB 0x00000001
#define MSG_PEEK 0x00000002
#define MSG_DONTROUTE 0x00000004
#define MSG_CTRUNC 0x00000008
#define MSG_TRUNC 0x00000020
#define MSG_EOR 0x00000080
#define MSG_WAITALL 0x00000100
#define MSG_NOSIGNAL 0x00004000
#define MSG_CMSG_CLOEXEC 0x40000000

#define AF_UNSPEC 0
#define AF_UNIX 1
#define AF_INET 2
#define AF_INET6 10

#define SHUT_RD 0
#define SHUT_WR 1
#define SHUT_RDWR 2

typedef __INT32_TYPE__ socklen_t;
typedef __UINT16_TYPE__ sa_family_t;
typedef __SIZE_TYPE__ size_t;
typedef __INT64_TYPE__ ssize_t;

struct sockaddr {
	sa_family_t sa_family;
	char sa_data[];
};

struct sockaddr_storage {
	sa_family_t ss_family;
	char __padding[128 - sizeof(long) - sizeof(sa_family_t)];
	unsigned long __align;
};

struct msghdr {
	void *msg_name;
	socklen_t msg_namelen;
	struct iovec *msg_iov;
	int msg_iovlen;
	void *msg_control;
	socklen_t msg_controllen;
	int msg_flags;
};

struct cmsghdr {
	socklen_t cmsg_len;
	int cmsg_level;
	int cmsg_type;
};

int accept(int, struct sockaddr *restrict, socklen_t *restrict);
int accept4(int, struct sockaddr *restrict, socklen_t *restrict, int);
int bind(int, const struct sockaddr *, socklen_t);
int connect(int, const struct sockaddr *, socklen_t);
int getpeername(int, struct sockaddr *restrict, socklen_t *restrict);
int getsockname(int, struct sockaddr *restrict, socklen_t *restrict);
int getsockopt(int, int, int, void *restrict, socklen_t *restrict);
int listen(int, int);
ssize_t recv(int, void *, size_t, int);
ssize_t recvfrom(int, void *restrict, size_t, int, struct sockaddr *restrict,
		 socklen_t *restrict);
ssize_t recvmsg(int, struct msghdr *, int);
ssize_t send(int, const void *, size_t, int);
ssize_t sendmsg(int, const struct msghdr *, int);
ssize_t sendto(int, const void *, size_t, int, const struct sockaddr *,
	       socklen_t);
int setsockopt(int, int, int, const void *, socklen_t);
int shutdown(int, int);
int sockatmark(int);
int socket(int, int, int);
int socketpair(int, int, int, int[2]);

#endif
