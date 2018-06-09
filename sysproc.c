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
  cprintf("\n hello\n");
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
  cprintf("\n width:%s \n",Title);

  int iX=0;
  int iY=0;
  Draw_Rect(window, iX, iY, iX + width, iY + height, RGB(100,200,80),0);
  Draw_Rect(window, iX, iY, iX + width, iY + height, RGB(100,100,100),1);
  Draw_Rect(window, iX, iY, iX + width - 1, iY + 21, RGB(0,0,0),1);
  Draw_Line(window, iX + width - 20, iY + 5, iX + width - 6, iY + 15, RGB(71,199,21));
  Draw_Line(window, iX + width - 20, iY + 15, iX + width - 6, iY + 5, RGB(71,199,21));
  Draw_Line(window, iX + width - 25, iY, iX + width - 25, iY + 21, RGB(71,199,21));
  Write_Char(window, iX + 6, iY + 3,RGB(255,255,255),RGB(255,255,255),Title,8);
 cprintf("height:%d",height);
 cprintf("width:%d",width);
  int i,j;
  for( i=0;i<height;i++)
  for( j=0;j<width;j++)
  {
    local_Draw_Point(x+j,y+i,window.wndBuffer[i*width+j]);
  }
  return 1;
}
// int sys_deletewindow(void)
// {

// }

