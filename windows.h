#ifndef WINDOWS_H
#define WINDOWS_H

#include "types.h"
#include "spinlock.h"
#define wndMAX 100
typedef struct Rectangle
{
    int x1;
    int x2;
    int y1;
    int y2;
}REC;
typedef struct windowStruct//struct to save the information of window
{
    ushort * wndBuffer;
    int x;
    int y;
    int width;
    int height;
    char * Title;
    int ID;
    int IsActivated;
}wnd;

typedef struct List
{
    wnd windowList[wndMAX];
    int num;
}wndList;







#endif