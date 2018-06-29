#include "queue.h"
#include "mouse.h"
#include "types.h"
#include "spinlock.h"
#include "x86.h"
#include "traps.h"
#include "VBE.h"
#include "Graphics.h"
static mouseM mouseSt={0,};
static queue mousequeue;
static mouseDT mousebuf[10000];
extern wnd EditorWnd;
extern wnd pbWnd;
BOOL JudoutbFull()
{
    if(inb(0x64)&0x01)
    {
        return 1;
    }
    else
    return 0;
}
BOOL scan()
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
    int i;
    uchar data;
     initqueue(&mousequeue,mousebuf,10000,sizeof(mouseDT));
     for( i=0;i<0xffff;i++)
     if(JudinbFull()==0)
        break;
    outb( 0x64 , 0xa8 );

    for( i=0;i<0xffff;i++)
     if(JudinbFull()==0)
        break;
    outb( 0x64 , 0x20 );

    for( i=0;i<0xffff;i++)
     if(JudoutbFull()==1)
        break;

    data=inb(0x60);
    data|=0x02;
    outb( 0x64 , 0x60 );

    for( i=0;i<0xffff;i++)
     if(JudinbFull()==0)
        break;
    outb(0x60,data);
    outb( 0x64 , 0xd4 );

    for( i=0;i<0xffff;i++)
     if(JudinbFull()==0)
        break;

    outb(0x60,0xf4);
    initlock(&mouseSt.SpinLock,"mouse");
    picenable(IRQ_MOUSE);
    ioapicenable(IRQ_MOUSE, 0);
    cprintf("Set mouse successfully");
}

//1024 768
int iX=512;
int iY=384;
BOOL JudmouseData()//whether there is mouse data in output buffer.
{
    if(inb(0x64)&0x20)
    {
        return 1;
    }
    return 0;
}
int tt=0;
uchar temp;

void movemouse()
{
  int x,y;
    if(getmousefromqueue(&temp,&x,&y)==1)
    {
    if(x>0)
    x=1;
    if(x<0)
    x=-1;
    if(y>0)
    y=1;
    if(y<0)
    y=-1;
  
    if(iX+x==0||iX+x==SCREEN_WIDTH-MOUSE_WIDTH)
    x=0;
    if(iY+y==0||iY+y==SCREEN_HEIGHT-MOUSE_HEIGHT)
    y=0;
    iX=iX+x;
    iY=iY+y;

    if(temp&MOUSE_LBUTTON)
    {
        mouseclick(iX,iY);
    }
    if(tt==0)
    {
        tt++;
    }
    else
    {
        repaint(iX-x,iY-y);
    }
    save_mouse(iX,iY);
    draw_mouse(iX,iY);
    }

}
void mouseintr()
{
    uchar temp;
    if(JudoutbFull()==1)
    {
     if(JudmouseData()==1)
     {
        temp=scan();
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
        mouseSt.byct=0;
        res=PutQueue(&mousequeue,&mouseSt.CurData);
        release(&(mouseSt.SpinLock));
    }
    return res;
}

BOOL getmousefromqueue(uchar* status,int* x,int* y)
{
   // cprintf("getmouse from queue\n");

    mouseDT data;
    uchar res;
    uchar* tp;
    if(JudQueEmpty(&mousequeue)==1)
    {
        return 0;
    }
   
    acquire(&mouseSt.SpinLock);
    res=GetQueue(&mousequeue,&data);
    release(&mouseSt.SpinLock);
   
    if(res==0)
    {
        return 0;
    }

      *status=data.flag &0x7;

    if ( (data.flag >> 7) & 0x1 || (data.flag >> 6) & 0x1) 
    {
        return -1;
    }

    *x=data.x&0xff;
    if(data.flag&0x10)
    {
    *x|=0xffffff00;
    }
    *y=data.y&0xff;
    if(data.flag&0x20)
    {
    *y|=0xffffff00;
    }
    *y=-*y;

    return 1;
}
