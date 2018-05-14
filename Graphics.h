#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "types.h"

typedef ushort  COLOR;


//#define RGB( r, g, b )      ( ( ( uchar )( r ) >> 3 ) << 11 | \
                ( ( ( uchar )( g ) >> 2 ) ) << 5 |  ( ( uchar )( b ) >> 3 ) )


int RGB(int r,int g,int b);
void Draw_Point( int iX, int iY, COLOR stColor );
void Draw_Line( int iX1, int iY1, int iX2, int iY2, COLOR stColor );
void Draw_Rect( int iX1, int iY1, int iX2, int iY2, COLOR stColor, int Fill );
void Write_Char(int iX,int iY,COLOR charColor,COLOR BackColor,const char* inputString,int length);
void Windows(int iX,int iY,int width,int height,const char* title);
void Draw_Mouse();
//void FillRect(void ) 

#endif 
