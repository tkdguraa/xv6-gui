#include "queue.h"
#include "mouse.h"
#include "types.h"
#include "spinlock.h"
#include "x86.h"
#include "traps.h"
#include "VBE.h"
#include "Graphics.h"
extern wnd EditorWnd;
extern wnd pbWnd;
int JudActivated=0;
extern int Actnum;
//Draw_Line(window, iX + width - 25, iY, iX + width - 25, iY + 21, RGB(255,255,255));
extern int tt;

void mouseclick(int iX,int iY)
{   
    if(JudActivated==0)
    {
         if((iX<520&&iX>400&&iY<670&&iY>550))//300 550,420 550
        {
            updateWnd(EditorWnd,0);
            JudActivated=1;
        }
        if((iX<640&&iX>520&&iY<670&&iY>550))//300 550,420 550
        {
             updateWnd(pbWnd,0);
             JudActivated=2;
        }
    }
    if(JudActivated!=0)
    {
        if(iX<(EditorWnd.x+EditorWnd.width)&&iX>(EditorWnd.x+EditorWnd.width-25)&&(EditorWnd.y<iY)&&(EditorWnd.y+21)>iY)
        {
        tt=0;
        reset();
        JudActivated=0;
        }
    }
    if(JudActivated==2)
    {
        startpaint(iX,iY);
    }
}