#include "syscall.h"
#include "traps.h"

#define SYSCALL(name) \
  .globl name; \
  name: \
    movl $SYS_ ## name, %eax; \
    int $T_SYSCALL; \
    ret

SYSCALL(fork)
SYSCALL(exit)
SYSCALL(wait)
SYSCALL(pipe)
SYSCALL(read)
SYSCALL(write)
SYSCALL(close)
SYSCALL(kill)
SYSCALL(exec)
SYSCALL(open)
SYSCALL(mknod)
SYSCALL(unlink)
SYSCALL(fstat)
SYSCALL(link)
SYSCALL(mkdir)
SYSCALL(chdir)
SYSCALL(dup)
SYSCALL(getpid)
SYSCALL(sbrk)
SYSCALL(sleep)
SYSCALL(uptime)
<<<<<<< HEAD
SYSCALL(loadimg)
SYSCALL(createwindow)
SYSCALL(deletewindow)
SYSCALL(signal)
SYSCALL(sigsend)
SYSCALL(cps)
SYSCALL(chpr)
=======
SYSCALL(signal)
SYSCALL(sigsend)
SYSCALL(cps)
SYSCALL(chpr)
>>>>>>> 47aa0ca669b5258a612c78e3005145ad951b7654
