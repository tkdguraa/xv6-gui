#include "bitmap.h"
#include "windows.h"
struct stat;
struct rtcdate;
struct RGBA;
struct RGB;
struct Window;
struct Point;
struct Size;
struct message;
struct Rect;

// system calls
int fork(void);
int exit(void) __attribute__((noreturn));
int wait(void);
int pipe(int*);
int write(int, void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(char*, int);
int mknod(char*, short, short);
int unlink(char*);
int fstat(int fd, struct stat*);
int link(char*, char*);
int mkdir(char*);
int chdir(char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
int signal(int signum, sighandler_t handler);
int sigsend(int pid, int signum);
int cps(void);
int chpr(int pid, int pr);

<<<<<<< HEAD

int loadimg(PICNODE pic,int height,int width, int x, int y);
int createwindow(wnd widnow);
int deletewindow();
// system calls for gui

=======
>>>>>>> 47aa0ca669b5258a612c78e3005145ad951b7654

// ulib.c
int stat(char*, struct stat*);
char* strcpy(char*, char*);
void *memmove(void*, void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void printf(int, char*, ...);
char* gets(char*, int max);
uint strlen(char*);
void* memset(void*, int, uint);
int atoi(const char*);

// umalloc.c
void* malloc(uint);
void free(void*);
int atoi(const char*);

<<<<<<< HEAD
int strfindfromback(char *s, const char c);
=======
int strfindfromback(char *s, const char c);
>>>>>>> 47aa0ca669b5258a612c78e3005145ad951b7654
