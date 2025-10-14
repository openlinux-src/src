#ifndef __STDIO_H
#define __STDIO_H

#ifndef NULL
#define NULL ((void *)0)
#endif

#define _IOFBF 2
#define _IOLBF 1
#define _IONBF 0

#undef SEEK_SET
#undef SEEK_CUR
#undef SEEK_END
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define BUFSIZ 4096

#define EOF (-1)

typedef struct __FILE FILE;
typedef __builtin_va_list va_list;
typedef __SIZE_TYPE__ size_t;

extern FILE *const stdin;
extern FILE *const stdout;
extern FILE *const stderr;

// TODO: sort it
char *ctermid(char *);
void clearerr(FILE *);
int fclose(FILE *);
int ferror(FILE *);
int fflush(FILE *);
int fputc(int, FILE *);
int fputs(const char *restrict, FILE *restrict);
size_t fread(void *restrict, size_t, size_t, FILE *restrict);
int fseek(FILE *, long, int);
size_t fwrite(const void *restrict, size_t, size_t, FILE *restrict);
void perror(const char *);
int printf(const char *restrict, ...);
int putc(int, FILE *);
int putchar(int);
int puts(const char *);
void setbuf(FILE *restrict, char *restrict);
int setvbuf(FILE *restrict, char *restrict, int, size_t);
int snprintf(char *restrict, size_t, const char *restrict, ...);
int vasprintf(char **restrict, const char *restrict, va_list);
int dprintf(int, const char *restrict, ...);
int vdprintf(int, const char *restrict, va_list);
int vfprintf(FILE *restrict, const char *restrict, va_list);
int vsnprintf(char *restrict, size_t, const char *restrict, va_list);
int getc(FILE *);
int fgetc(FILE *);
int getchar(void);
int rename(const char *, const char *);
int fileno(FILE *);

#endif
