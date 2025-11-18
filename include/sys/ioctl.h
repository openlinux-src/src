#ifndef __SYS_IOCTL_H
#define __SYS_IOCTL_H

#define TIOCGWINSZ 0x5413

int ioctl(int, int, ...);

#endif
