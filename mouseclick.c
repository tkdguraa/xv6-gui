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
extern wnd test;
int JudActivated=0;
extern int Actnum;
//draw_line(window, iX + width - 25, iY, iX + width - 25, iY + 21, RGB(255,255,255));
extern int tt;
extern int pencolor;
extern wndList List;
void mouseclick(int iX,int iY)
{   
         if((iX<470&&iX>350&&iY<670&&iY>550))//300 550,420 550
        {
	        if(EditorWnd.IsActivated==0)//open editor
            {
            setup_editor(EditorWnd);
            updateWnd(EditorWnd,0);
            start_editor(EditorWnd);
            EditorWnd.IsActivated=1;
            List.windowList[List.num]=EditorWnd;
            List.num++;
            }
        }
        if((iX<590&&iX>470&&iY<670&&iY>550))//300 550,420 550
        {
             if(pbWnd.IsActivated==0)//oepn paintboard
             {
             setup_paintB(pbWnd);
             updateWnd(pbWnd,0);
             pbWnd.IsActivated=1;
             List.windowList[List.num]=pbWnd;
             List.num++;
             }
        }
        if(iX<670&&iX>590&&iY<670&&iY>550)
        {
            if(test.IsActivated==0)
             {
             updateWnd(test,0);
             test.IsActivated=1;
             List.windowList[List.num]=test;
             List.num++;
             }
        }
    overlap(iX,iY);
    closewindow(iX,iY);
    if(pbWnd.IsActivated==1)
    {
        start_paint(iX,iY);
    }
}

// void movewindow(int iX,int iY)
// {
//     if(iX<(List.windowList[i].x+List.windowList[i].width-25)&&iX>(List.windowList[i].x)
//     &&(List.windowList[i].y<iY)&&(List.windowList[i].y+21)>iY)
//     {

//     }
// }

void overlap(int iX,int iY)
{
    //Judge that which window did I click and let that window on the top
    int i,pt;
    wnd temp;
    int IsClicked=0;
    for(i=0;i<List.num;i++)
    {
        if(iX>List.windowList[i].x&&iX<(List.windowList[i].x+List.windowList[i].width)&&iY>List.windowList[i].y&&iY<List.windowList[i].y+List.windowList[i].height)
        {
            temp=List.windowList[i];
            pt=i;
            IsClicked=1;
        }
    }
      if((temp.ID!=List.windowList[List.num-1].ID)&&IsClicked==1)//If the window which I clicked isn't on the top 
       {
           List.windowList[pt]=List.windowList[List.num-1];
           List.windowList[List.num-1]=temp;
           reset();
       }
}

void closewindow(int iX,int iY)
{
    int i;
 
    i=List.num-1;//close window 
    
         if(iX<(List.windowList[i].x+List.windowList[i].width)&&iX>(List.windowList[i].x+List.windowList[i].width-25)&&(List.windowList[i].y<iY)&&(List.windowList[i].y+21)>iY)
        {
        tt=0;
        cprintf("ID: %d\n",List.windowList[i].ID);
        pencolor=0;   
        List.windowList[i].IsActivated=0;
         if(List.windowList[i].ID==1)
         {
            EditorWnd.IsActivated=0;
         }
            if(List.windowList[i].ID==2)
            pbWnd.IsActivated=0;
           
            if(List.windowList[i].ID==3)
           {
            test.IsActivated=0;
           }
        List.num--; 
        reset();
        }
}
