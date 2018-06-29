#include "Graphics.h"
#include "VBE.h"
#include "character.h"
#include "mouse.h"
#include "windows.h"
#include "defs.h"
#include "bitmap.h"
extern ushort* BACKGROUND;
ushort savemouse[20][20];
int ct1=0;
int ct2=0;
wndList WList;
static uchar MouseBuf[MOUSE_WIDTH * MOUSE_HEIGHT ] =
{
    1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1,
    0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1,
    0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 0,
    0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 0, 0, 0,
    0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 1, 0, 0, 0,
    0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0, 1, 2, 2, 2, 2, 2, 1, 0, 0,
    0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 1, 0,
    0, 0, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 1,
    0, 0, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 1, 0,
    0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 0, 0,
    0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
};//shape of mouse
int pencolor=0;
void draw_mouse(int iX,int iY)
{
    int i,j;
    uchar* loadMouse;
    loadMouse=MouseBuf;
    for(i=0;i<MOUSE_HEIGHT;i++)
    {
        for(j=0;j<MOUSE_WIDTH;j++)
        {
            switch(*loadMouse)
            {
                case 0:
                break;

                case 1:
                {
                local_draw_point(iX+j,iY+i,RGB(255,255,255));
                }
                break;

                case 2:
                {
                local_draw_point(iX+j,iY+i,pencolor);
                }
                break;
            }
            loadMouse++;
        }
    }
}

void initList(wndList List)
{
    List.num=0;
}
 void draw_point( wnd window, int iX, int iY, COLOR stColor )
{
    //*(((COLOR*)VESA_ADDR) + SCREEN_WIDTH * iY + iX ) = stColor;
    window.wndBuffer[window.width*iY+iX]=stColor;
} 
void local_draw_point(int iX, int iY, COLOR stColor )
{
    *(((COLOR*)VESA_ADDR) + SCREEN_WIDTH * iY + iX ) = stColor;
} 
 void save_mouse( int iX, int iY )
 {
     int i,j;
     for(i=0;i<20;i++)
        for(j=0;j<20;j++)
        {
            savemouse[i][j]=*(((COLOR*)VESA_ADDR) + SCREEN_WIDTH * (iY+i) + iX+j);
        }
 }
int RGB(int r,int g,int b)
{
	return ((b / 8)+((g / 4)<<5)+((r / 8)<<11));
}
void repaint( int iX, int iY)
{
   int i,j;
    for(i=0;i<20;i++)
        for(j=0;j<20;j++)
        {
           *(((COLOR*)VESA_ADDR) + SCREEN_WIDTH * (iY+i) + iX+j ) = savemouse[i][j];
        }
    
}


void draw_line( wnd window,int iX1, int iY1, int iX2, int iY2, COLOR stColor )
{
    int iDeltaX, iDeltaY;
    int iError = 0;
    int iDeltaError;
    int iX, iY;
    int iStepX, iStepY;

    iDeltaX = iX2 - iX1;
    iDeltaY = iY2 - iY1;

    if( iDeltaX < 0 ) 
    {
        iDeltaX = -iDeltaX; 
        iStepX = -1; 
    } 
    else 
    { 
        iStepX = 1; 
    }
    if( iDeltaY < 0 ) 
    {
        iDeltaY = -iDeltaY; 
        iStepY = -1; 
    } 
    else 
    {
        iStepY = 1; 
    }
    if( iDeltaX > iDeltaY )
    {
        iDeltaError = iDeltaY << 1;
        iY = iY1;
        for( iX = iX1 ; iX != iX2 ; iX += iStepX )
        {
            draw_point( window, iX, iY, stColor );
            iError += iDeltaError;

            if( iError >= iDeltaX )
            {
                iY += iStepY;
                iError -= iDeltaX << 1;
            }
        }
        draw_point( window, iX, iY, stColor );
    }
    else
    {
        iDeltaError = iDeltaX << 1;
        iX = iX1;
        for( iY = iY1 ; iY != iY2 ; iY += iStepY )
        {
            draw_point( window, iX, iY, stColor );
            iError += iDeltaError;
            if( iError >= iDeltaY )
            {
                iX += iStepX;
                iError -= iDeltaY << 1;
            }
        }
        draw_point( window, iX, iY, stColor );
    }
}
void draw_rect(wnd window,int iX1, int iY1, int iX2, int iY2, COLOR stColor, int Fill )
{
    int width;
    int iTemp;
    int iY;
    int iStepY;
    int i;
        draw_line( window, iX1, iY1, iX2, iY1, stColor );
        draw_line( window, iX1, iY1, iX1, iY2, stColor );
        draw_line( window, iX2, iY1, iX2, iY2, stColor );
        draw_line( window, iX1, iY2, iX2, iY2, stColor );
    if(Fill==1)
    {
        if(iY1>iY2)
        {
            for(i=0; i<iY1-iY2; i++)
            draw_line( window, iX1, iY2+i, iX2,iY2+i,stColor);
        }
        else
        {
            for( i=0; i<iY2-iY1; i++)
            draw_line( window, iX1, iY1+i, iX2,iY1+i,stColor);
        }
    }
}

void write_char( wnd window, int iX, int iY, COLOR CharColor, COLOR BackColor, const char inputString)
{
    int x,y,i,j;
    unsigned char word;
    int start;
   
    x=iX;
  
        y = iY*window.width;
        start = inputString * CHAR_HEIGHT;
        for( j = 0; j < CHAR_HEIGHT; j++)
        {
            word = Characters[start++];
            for( i = 0; i < CHAR_WIDTH; i++)
            {
                if(word&(0x01<<(CHAR_WIDTH-i-1)))
                {
                    window.wndBuffer[x+y+i]=BackColor;
                }
            }
            y=y+window.width;
        }
        x=x+CHAR_WIDTH; 
}
void local_write_char( int iX, int iY, COLOR CharColor, COLOR BackColor, const char* inputString, int length )
{
    int x,y,i,j,k;
    unsigned char word;
    int start;
   
    x=iX;
  
    for( k = 0 ; k < length ; k++)
    {
        y = iY*SCREEN_WIDTH;
        start = inputString[k] * CHAR_HEIGHT;
        for( j = 0; j < CHAR_HEIGHT; j++)
        {
            word = Characters[start++];
            for( i = 0; i < CHAR_WIDTH; i++)
            {
                if(word&(0x01<<(CHAR_WIDTH-i-1)))
                {
                    VESA_ADDR[x+y+i]=BackColor;
                }
            }
            y=y+SCREEN_WIDTH;
        }
        x=x+CHAR_WIDTH; 
    }
}

void updateWnd(wnd window,int JudActivated)
{
  int iX=0;
  int iY=0;
  int width,height;
  int x,y;
  width=window.width;
  height=window.height;
  x=window.x;
  y=window.y;

//   if(JudActivated==0)
//   {
//   draw_rect(window, iX, iY, iX + width, iY + height, RGB(255,255,255),1);
//   draw_rect(window, iX, iY, iX + width, iY + height, RGB(60,60,60),0);
//   draw_rect(window, iX, iY, iX + width - 1, iY + 21, RGB(60,60,60),1);
//   draw_line(window, iX + width - 20, iY + 5, iX + width - 6, iY + 15, RGB(255,255,255));
//   draw_line(window, iX + width - 20, iY + 15, iX + width - 6, iY + 5, RGB(255,255,255));
//   draw_line(window, iX + width - 25, iY, iX + width - 25, iY + 21, RGB(255,255,255));
//   write_char(window, iX + 6, iY + 3,RGB(255,255,255),RGB(255,255,255),window.Title,strlen(window.Title));
//   }
  int i,j;
  for( i=0;i<=height;i++)
  for( j=0;j<=width;j++)
  {
    local_draw_point(x+j,y+i,window.wndBuffer[i*width+j]);
  }
}

// void Windows( int iX, int iY, int width, int height,const char* title)
// {
//     draw_rect( iX, iY, iX + width, iY + height, RGB(100,200,80),0);
//     draw_rect( iX, iY, iX + width, iY + height, RGB(100,100,100),1);
//     draw_rect( iX, iY, iX + width - 1, iY + 21, RGB(0,0,0),1);
//     draw_line( iX + width - 20, iY + 5, iX + width - 6, iY + 15, RGB(71,199,21));
//     draw_line( iX + width - 20, iY + 15, iX + width - 6, iY + 5, RGB(71,199,21));
//     draw_line( iX + width - 25, iY, iX + width - 25, iY + 21, RGB(71,199,21));
//     write_char( iX + 6, iY + 3,RGB(255,255,255),RGB(255,255,255),title,6);
// }


