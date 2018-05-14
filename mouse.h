#ifndef MOUSE_H
#define MOUSE_H

#include  "types.h"
#include  "spinlock.h"
#define MOUSE_WIDTH 20
#define MOUSE_HEIGHT 20
typedef struct mousemove
{
    uchar flag;
    uchar x;
    uchar y;
}mouseDT;
typedef struct mouse
{
    spinlock SpinLock;
    int byct;
    mouseDT CurData;
}mouseM;
BOOL getmousefromqueue(uchar* status,int* x,int* y);
BOOL putmouseintoqueue(uchar mousedata);
void mouseinit();
void mouseintr();
void movemouse();
uchar scan();
BOOL JudinbFull();
BOOL JudoutbFull();
#endif