#include "queue.h"
#include "mouse.h"
#include "types.h"
#include "spinlock.h"
#include "x86.h"
#include "traps.h"
#include "VBE.h"
static mouseM mouseSt={0,};
static queue mousequeue;
static mouseDT mousebuf[101];

BOOL JudoutbFull()
{
    if(inb(0x64)&0x01)
    {
        return 1;
    }
    else
    return 0;
}
uchar scan()
{
    while(JudoutbFull()==0)
    {
        ;
    }
    return inb(0x60);
}
BOOL JudinbFull()
{
    if(inb(0x64)&0x02)
    {
        return 1;
    }
    else
    return 0;
}

BOOL waitAFK()
{
    int i,j;
    uchar data;
    BOOL res=0;

    for(j=0;j<100;j++)
    {
        for(i=0;i<0xffff;i++)
        {
            if(JudoutbFull()==1)
            break;
        }
    }
    data=inb(0x60);
    putmouseintoqueue(data);
    return res;
}
void mouseinit()
{
    uchar data;
     for(int i=0;i<0xffff;i++)
     if(JudinbFull()==0)
        break;
    outb( 0x64 , 0xa8 );

    for(int i=0;i<0xffff;i++)
     if(JudinbFull()==0)
        break;
    outb( 0x64 , 0x20 );

    for(int i=0;i<0xffff;i++)
     if(JudoutbFull()==1)
        break;

    data=inb(0x60);
    data|=0x02;
    outb( 0x64 , 0x60 );

    for(int i=0;i<0xffff;i++)
     if(JudinbFull()==0)
        break;
    outb(0x60,data);
     outb( 0x64 , 0xd4 );

    for(int i=0;i<0xffff;i++)
     if(JudinbFull()==0)
        break;

    outb(0x60,0xf4);
    initlock(&mouseSt.SpinLock,"mouse");
    picenable(IRQ_MOUSE);
    ioapicenable(IRQ_MOUSE, 0);
    initqueue(&mousequeue,mousebuf,101,sizeof(mouseDT));
}
int iX=514;
int iY=300;
BOOL JudmouseData()//whether there is mouse data in output buffer.
{
    if(inb(0x64)&0x20)
    {
        return 1;
    }
    return 0;
}
void movemouse()
{
  uchar temp;
  int x,y;
  //while(1)
  {
    if(getmousefromqueue(&temp,&x,&y)==1)
    {
    Draw_Rect(iX,iY,iX+MOUSE_WIDTH,iY+MOUSE_HEIGHT,RGB(255,255,255),1);
    iX=iX+x;
    iY=iY+y;
    if(iX<0)
    iX=0;
    else if(iX>SCREEN_WIDTH)
    iX=SCREEN_WIDTH;
    if(iY<0)
    iY=0;
    else if(iY>SCREEN_HEIGHT)
    iY=SCREEN_HEIGHT;
    Draw_Mouse(iX,iY);
    }
  }
}
void mouseintr()
{
    uchar temp;
    if(JudoutbFull()==1)
    {
     temp=scan();
     if(JudmouseData()==1)
     {
        putmouseintoqueue(temp);
        movemouse();
     }
    }
}
BOOL putmouseintoqueue(uchar mousedata)
{
    uchar res;
   // cprintf("put mouse into queue\n");
    switch(mouseSt.byct)
    {
        case 0:
        mouseSt.CurData.flag=mousedata;
        mouseSt.byct++;
        break;
        case 1:
        mouseSt.CurData.x=mousedata;
        mouseSt.byct++;
        break;
        case 2:
        mouseSt.CurData.y=mousedata;
        mouseSt.byct++;
        break;

        default:
        mouseSt.byct=0;
        break;
    }
    if(mouseSt.byct>=3)
    {
        acquire(&(mouseSt.SpinLock));
        res=PutQueue(&mousequeue,&mouseSt.CurData);
        release(&(mouseSt.SpinLock));
        mouseSt.byct=0;
    }
    return res;
}
BOOL getmousefromqueue(uchar* status,int* x,int* y)
{
   // cprintf("getmouse from queue\n");
    mouseDT data;
    uchar res;
    acquire(&mouseSt.SpinLock);
    res=GetQueue(&mousequeue,&data);
    release(&mouseSt.SpinLock);
    if(res==0)
    {
        return 0;
    }

    status=data.flag &07;
    *x=data.x&0xff;
    if(data.flag&0x10)
    *x|=0xffffff00;
    y=data.y&0xff;
    if(data.flag&0x20)
    *y|=0xffffff00;
    y=-*y;
    return 1;
}
