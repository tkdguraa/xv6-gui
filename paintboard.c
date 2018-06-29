#include "Graphics.h"
#include "VBE.h"

extern wnd pbWnd;
extern int pencolor;

void setup_paintB(wnd Window)
{
    int i,j;
    for(i=0;i<20;i++)
        for(j=0;j<20;j++)
            Window.wndBuffer[Window.width*(i+21)+Window.width-21+j]=RGB(255,0,0);
    draw_rect(Window,Window.width-21,21,Window.width-1,40,RGB(0,0,0),0);

    for(i = 0;i < 20;i++)
        for(j = 0;j < 20;j++)
            Window.wndBuffer[Window.width*(i+41)+Window.width-21+j]=RGB(0,255,0);
    draw_rect(Window,Window.width-21,41,Window.width-1,60,RGB(0,0,0),0);

    for(i = 0;i < 20;i++)
        for(j = 0;j < 20;j++)
            Window.wndBuffer[Window.width*(i+61)+Window.width-21+j]=RGB(0,0,255);
    draw_rect(Window,Window.width-21,61,Window.width-1,80,RGB(0,0,0),0);

    write_char(Window,Window.width-12,81,RGB(0,0,0),RGB(0,0,0),'E');
    draw_rect(Window,Window.width-21,81,Window.width-1,100,RGB(0,0,0),0);
}
//draw_line(window, iX + width - 25, iY, iX + width - 25, iY + 21, RGB(255,255,255));
//  draw_rect(Window,Window.width-21,21,Window.width-1,40,RGB(0,0,0),0);
void start_paint(int iX,int iY)
{
    if(iX<(pbWnd.x+pbWnd.width)&&iX>(pbWnd.x)&&iY>(pbWnd.y+21)&&iY<(pbWnd.y+pbWnd.height))
    {
        if(iX > (pbWnd.x+pbWnd.width-21) && iX < pbWnd.x+pbWnd.width && iY < (100+pbWnd.y) && iY > (pbWnd.y+21))
        {
            if(iY < pbWnd.y+40)
                pencolor=RGB(255,0,0);

            if(iY < pbWnd.y+60 && iY > pbWnd.y+40)
                pencolor=RGB(0,255,0);

            if(iY < pbWnd.y+80 && iY > pbWnd.y+60)
                pencolor=RGB(0,0,255);

            if(iY < pbWnd.y+100 && iY > pbWnd.y+80)
                pencolor=RGB(60,60,60);

        } 
        else
            local_draw_point(iX-1,iY-1,pencolor);
        
    }
    
}
