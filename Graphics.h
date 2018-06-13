#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "types.h"
#include "bitmap.h"
#include "windows.h"
typedef ushort  COLOR;


// #define RGB( r, g, b )      ( ( ( uchar )( r ) >> 3 ) << 11 | \
//                 ( ( ( uchar )( g ) >> 2 ) ) << 5 |  ( ( uchar )( b ) >> 3 ) )


int RGB(int r,int g,int b);
void Draw_Point( wnd window,int iX, int iY, COLOR stColor );
void local_Draw_Point(int iX, int iY, COLOR stColor );
void local_Write_Char( int iX, int iY, COLOR CharColor, COLOR BackColor, const char* inputString, int length );
void Draw_Line( wnd window,int iX1, int iY1, int iX2, int iY2, COLOR stColor );
void Draw_Rect( wnd window,int iX1, int iY1, int iX2, int iY2, COLOR stColor, int Fill );
void Write_Char( wnd window,int iX,int iY,COLOR charColor,COLOR BackColor,const char* inputString,int length);
void Windows(int iX,int iY,int width,int height,const char* title);
void Draw_Mouse();
void draw_picture(PICNODE pic, int x, int y);
void save_point( int iX,int iY);
void save_mouse( int iX, int iY );
void repaint(int iX, int iY);
void updatewnd(wnd Window,int JudActivated);
void mouseclick(int iX,int iY);
void reset();
void startpaint(int x,int y);
//void FillRect(void ) 

#endif 
