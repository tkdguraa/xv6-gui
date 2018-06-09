#include "types.h"
#include "stat.h"
#include "user.h"
#include "bitmap.h"
#include "Graphics.h"
#include "fcntl.h"
#include "mouse.h"
//120x120 1024x768
//extern uchar temp;
int main(int argc, char *argv[])
{  
    PICNODE bgPic;
    PICNODE textedit;
    loadBitmap(&bgPic, "desk.bmp"); 
    set_icon_alpha(&bgPic);
    loadimg(bgPic,0,0,0,0);

    loadBitmap(&textedit,"textedit.bmp");
    set_icon_alpha(&textedit);
    loadimg(textedit,350,450,350,450);
    int pid;
    pid=fork();

    printf(1,"%d\n",fork);
   
   
    wnd txtwindow;
    txtwindow.wndBuffer=(ushort*)malloc(sizeof(ushort)*205*305);
    txtwindow.width=200;
    txtwindow.height=300;
    txtwindow.x=300;
    txtwindow.y=200;
    txtwindow.Title="textedit";
    createwindow(txtwindow);

    while(1);;
}   