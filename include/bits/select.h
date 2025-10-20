#ifndef __BITS_SELECT_H
#define __BITS_SELECT_H

#ifndef __BITS_SELECT_H_
#error "Internal header — include the public API header instead."
#endif

#define FD_SETSIZE 1024

#define FD_ZERO(set)                                                     \
	do {                                                             \
		for (size_t i = 0;                                       \
		     i < sizeof((set)->fds_bits) / sizeof(fd_mask); i++) \
			(set)->fds_bits[i] = 0;                          \
	} while (0)

#define FD_SET(fd, set)                                   \
	((set)->fds_bits[(fd) / (8 * sizeof(fd_mask))] |= \
	 (1UL << ((fd) % (8 * sizeof(fd_mask)))))

#define FD_CLR(fd, set)                                   \
	((set)->fds_bits[(fd) / (8 * sizeof(fd_mask))] &= \
	 ~(1UL << ((fd) % (8 * sizeof(fd_mask)))))

#define FD_ISSET(fd, set)                                 \
	(((set)->fds_bits[(fd) / (8 * sizeof(fd_mask))] & \
	  (1UL << ((fd) % (8 * sizeof(fd_mask))))) != 0)

typedef __INT64_TYPE__ time_t;
typedef __INT64_TYPE__ suseconds_t;

typedef struct {
	unsigned long fds_bits[1024 / (8 * sizeof(unsigned long))];
} fd_set;

struct timeval {
	time_t tv_sec;
	suseconds_t tv_usec;
};

#endif
