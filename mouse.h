#ifndef MOUSE_H
#define MOUSE_H

#include  "types.h"
#include  "spinlock.h"
#define MOUSE_WIDTH 20
#define MOUSE_HEIGHT 20
#define MOUSE_LBUTTON   0x01
#define MOUSE_RBUTTON   0x02
#define MOUSE_MBUTTON   0x04

#pragma pack( push, 1 )
typedef struct mousemove
{
    uchar flag;
    uchar x;
    uchar y;
    int xoverflow;
    int yoverflow;
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
BOOL scan();
BOOL JudinbFull();
BOOL JudoutbFull();
#endif