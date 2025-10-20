#ifndef __DIRENT_H
#define __DIRENT_H

#define DT_UNKNOWN 0x0
#define DT_FIFO	   0x1
#define DT_CHR	   0x2
#define DT_DIR	   0x4
#define DT_BLK	   0x6
#define DT_REG	   0x8
#define DT_LNK	   0xa
#define DT_SOCK	   0xc
#define DT_MQ	   0xe
#define DT_SEM	   0x10
#define DT_SHM	   0x12
#define DT_TMO	   0x14

typedef struct __DIR DIR;
typedef __UINT64_TYPE__ ino_t;
typedef __UINT32_TYPE__ reclen_t;
typedef __SIZE_TYPE__ size_t;
typedef __INT64_TYPE__ ssize_t;

struct dirent {
	ino_t d_ino;
	char d_name[257]; /* NAME_MAX + 1 */
};

struct posix_dent {
	ino_t d_ino;
	reclen_t d_reclen;
	unsigned char d_type;
	char d_name[];
};

int alphasort(const struct dirent **, const struct dirent **);
int closedir(DIR *);
int dirfd(DIR *);
DIR *fdopendir(int);
DIR *opendir(const char *);
ssize_t posix_getdents(int, void *, size_t, int);
struct dirent *readdir(DIR *);
int readdir_r(DIR *restrict, struct dirent *restrict, struct dirent **restrict);
void rewinddir(DIR *);
int scandir(const char *, struct dirent ***, int (*)(const struct dirent *),
	    int (*)(const struct dirent **, const struct dirent **));
void seekdir(DIR *, long);
long telldir(DIR *);

#endif
