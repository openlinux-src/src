#ifndef __SYS_MOUNT_H
#define __SYS_MOUNT_H

int mount(const char *, const char *, const char *, unsigned long,
	  const void *);
int umount(const char *);
int umount2(const char *, int);

#endif
