#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "bitmap.h"
#include "windows.h"
wnd EditorWnd;
wnd pbWnd;
int cnt=0;
PICNODE bgpic;
PICNODE textedit;
PICNODE paintboard;

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;
  
  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
int iconcnt=0;
int
sys_loadimg(void)
{
  PICNODE pic;
  int x, y, height, width;
  if (argptr(0, (void*)&pic, sizeof(PICNODE)))
    return -1;    
  if(argint(1, &height) < 0)
    return;
  if(argint(2, &width) < 0)
    return;
  if(argint(3, &x) < 0)
    return -1;
  if(argint(4, &y) < 0)
    return -1;

  return loadimg(pic,height,width,x,y);
}
int sys_createwindow(void)
{
  wnd window;
  
  if(argptr(0,(void*)&window,sizeof(wnd)))
  return -1;
  int x,y,height,width;
  char * Title;
  if(argint(1,&x)<0)
  return;
    window.x=x;
  if(argint(2,&y)<0)
  return;
  window.y=y;
  if(argint(3,&width)<0)
  return;
   window.width=width;
  if(argint(4,&height)<0)
  return;
   window.height=height;
 if(argstr(5,&Title)<0)
  return;
  window.Title=Title;

  if(cnt==0)
  {
  EditorWnd=window;
  cprintf("%s\n",window.Title);
  }
  if(cnt==1)
  {
  pbWnd=window;
  cprintf("%s",window.Title);
  }
  cnt++;
}

  
// int sys_deletewindow(void)
// {

// }


int
sys_signal(void)
{
  int signum;
  int handler = 0;

  if (argint(0, &signum) < 0 || argint(1, &handler) < 0)
    return -1;

  return signal(signum, (sighandler_t)handler);
}

int
sys_sigsend(void) 
{
  int pid;
  int signum;

  if(argint(0, &pid) < 0 || argint(1, &signum) < 0)
    return -1;

  return sigsend(pid, signum);
}

int
sys_cps(void)
{
  return cps();
}

int
sys_chpr(void)
{
  int pid, pr;
  if(argint(0, &pid) < 0)
    return -1;
  if(argint(1, &pr) < 0)
    return -1;

  return chpr(pid, pr);
}
