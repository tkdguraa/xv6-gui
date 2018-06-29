#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "types.h"
#include "bitmap.h"
#include "windows.h"
typedef ushort  COLOR;


// #define RGB( r, g, b )      ( ( ( uchar )( r ) >> 3 ) << 11 | \
//                 ( ( ( uchar )( g ) >> 2 ) ) << 5 |  ( ( uchar )( b ) >> 3 ) )


int RGB(int r,int g,int b);
void draw_point( wnd window,int iX, int iY, COLOR stColor );
void local_draw_point(int iX, int iY, COLOR stColor );
void local_write_char( int iX, int iY, COLOR CharColor, COLOR BackColor, const char* inputString, int length );
void draw_line( wnd window,int iX1, int iY1, int iX2, int iY2, COLOR stColor );
void draw_rect( wnd window,int iX1, int iY1, int iX2, int iY2, COLOR stColor, int Fill );
void write_char( wnd window,int iX,int iY,COLOR charColor,COLOR BackColor,const char inputString);
void draw_mouse();
void draw_picture(PICNODE pic, int x, int y);
void save_point( int iX,int iY);
void save_mouse( int iX, int iY );
void repaint(int iX, int iY);
void updatewnd(wnd Window,int JudActivated);
void mouseclick(int iX,int iY);
void reset();
void start_paint(int x,int y);
void setup_paintB(wnd Window);
void setup_editor(wnd Window);
void start_editor(wnd Window);
void initList(wndList List);    
//void FillRect(void ) 

#endif 
