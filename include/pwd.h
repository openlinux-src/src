#ifndef __PWD_H
#define __PWD_H

typedef __SIZE_TYPE__ size_t;
typedef __UINT32_TYPE__ gid_t;
typedef __UINT32_TYPE__ uid_t;

struct passwd {
	char *pw_name;
	uid_t pw_uid;
	gid_t pw_gid;
	char *pw_dir;
	char *pw_shell;
};

void endpwent(void);
struct passwd *getpwent(void);
struct passwd *getpwnam(const char *);
int getpwnam_r(const char *, struct passwd *, char *, size_t, struct passwd **);
struct passwd *getpwuid(uid_t);
int getpwuid_r(uid_t, struct passwd *, char *, size_t, struct passwd **);
void setpwent(void);

#endif
