#include "windows.h"
#include "defs.h"
#include "types.h"
#include "user.h"

WndManager WndM;

static void WndMananagerinit()
{
    void* WndAddress;
    memset(&WndM,0,sizeof(WndManager));
    WndAddress=(void*)malloc(sizeof(Wnd)*200);
    WndM.StartAddress=(Wnd*)WndAddress;
    for(int i=0;i<200;i++)//allocate the ID to window
    {
        WndM.StartAddress[i].Link.ID=i;
    }
    WndM.Max=200;
    WndM.AllocatedCt=1;
}
static Wnd* AllocateWnd()
{
    int i;
    Wnd* EmptyWindow;
    acquire(&WndM.lock);
    for(i=0;i<WndM.Max;i++)
    {
        if(WndM.StartAddress[i].Link.ID>>32==0)//if equal to 0,it means that this is empty window
        {
            EmptyWindow=&(WndM.StartAddress[i]);
            break;
        }
    }
    EmptyWindow->Link.ID=((uchar)WndM.AllocatedCt<<32|i);//mark that this window is not empty now
    WndM.Use++;//number of window which is being used
    WndM.AllocatedCt++;//number of window which is allocated
    release(&WndM.lock);
    return EmptyWindow;
}