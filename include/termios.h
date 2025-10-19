#ifndef __TERMIOS_H
#define __TERMIOS_H

#define NCCS 32

#define VINTR  0
#define VQUIT  1
#define VERASE 2
#define VKILL  3
#define VEOF   4
#define VTIME  5
#define VMIN   6
#define VSTART 8
#define VSTOP  9
#define VSUSP  10
#define VEOL   11

#define IGNBRK 0x0001
#define BRKINT 0x0002
#define IGNPAR 0x0004
#define PARMRK 0x0008
#define INPCK  0x0010
#define ISTRIP 0x0020
#define INLCR  0x0040
#define IGNCR  0x0080
#define ICRNL  0x1000
#define IXON   0x0400
#define IXANY  0x0800
#define IXOFF  0x1000

#define OPOST  0x0001
#define ONLCR  0x0004
#define OCRNL  0x0008
#define ONOCR  0x0010
#define ONLRET 0x0020
#define OFILL  0x0040
#define OFDEL  0x0080
#define NLDLY  0x0100
#define CRDLY  0x0600
#define TABDLY 0x1800
#define BSDLY  0x2000
#define VTDLY  0x4000
#define FFDLY  0x8000

#define B0     0000000
#define B50    0000001
#define B75    0000002
#define B110   0000003
#define B134   0000004
#define B150   0000005
#define B200   0000006
#define B300   0000007
#define B600   0000010
#define B1200  0000011
#define B1800  0000012
#define B2400  0000013
#define B4800  0000014
#define B9600  0000015
#define B19200 0000016
#define B38400 0000017

#define CSIZE  0x00000030
#define CS5    0x00000000
#define CS6    0x00000010
#define CS7    0x00000020
#define CS8    0x00000030
#define CSTOPB 0x00000040
#define CREAD  0x00000080
#define PARENB 0x00000100
#define PARODD 0x00000200
#define HUPCL  0x00000400
#define CLOCAL 0x00000800

#define ISIG   0x00001
#define ICANON 0x00002
#define ECHO   0x00008
#define ECHOE  0x00010
#define ECHOK  0x00020
#define ECHONL 0x00040
#define NOFLSH 0x00080
#define TOSTOP 0x00100
#define IEXTEN 0x08000

#define TCSANOW	  0
#define TCSADRAIN 1
#define TCSAFLUSH 2

#define TCIFLUSH  0
#define TCOFLUSH  1
#define TCIOFLUSH 2

#define TCOOFF 0
#define TCOON  1
#define TCIOFF 2
#define TCION  3

typedef __INT64_TYPE__ pid_t;
typedef unsigned char cc_t;
typedef unsigned int speed_t;
typedef unsigned int tcflag_t;

struct winsize {
	unsigned short ws_row;
	unsigned short ws_col;
};

struct termios {
	tcflag_t c_iflag;
	tcflag_t c_oflag;
	tcflag_t c_cflag;
	tcflag_t c_lflag;
	cc_t c_cc[NCCS];
};

speed_t cfgetispeed(const struct termios *);
speed_t cfgetospeed(const struct termios *);
int cfsetispeed(struct termios *, speed_t);
int cfsetospeed(struct termios *, speed_t);
int tcdrain(int);
int tcflow(int, int);
int tcflush(int, int);
int tcgetattr(int, struct termios *);
pid_t tcgetsid(int);
int tcgetwinsize(int, struct winsize *);
int tcsendbreak(int, int);
int tcsetattr(int, int, const struct termios *);
int tcsetwinsize(int, const struct winsize *);

#endif
