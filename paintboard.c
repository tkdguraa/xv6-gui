#include "Graphics.h"
#include "VBE.h"

extern wnd pbWnd;
int pencolor=0;
void startpaint(int iX,int iY)
{
    if(iX<(pbWnd.x+pbWnd.width)&&iX>(pbWnd.x)&&iY>(pbWnd.y+21)&&iY<(pbWnd.y+pbWnd.height))
    {
        local_Draw_Point(iX-1,iY-1,pencolor);
    }
    
}