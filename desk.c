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
    PICNODE paintboard;
    loadBitmap(&bgPic, "desk.bmp"); 
    set_icon_alpha(&bgPic);
    loadimg(bgPic,0,0,0,0);

    loadBitmap(&textedit,"textedit.bmp");
    set_icon_alpha(&textedit);
    loadimg(textedit,400,550,400,550);

    loadBitmap(&paintboard,"paintboard.bmp");
    set_icon_alpha(&paintboard);
    loadimg(paintboard,520,550,520,550);

    wnd txtwindow;
    txtwindow.wndBuffer=(ushort*)malloc(sizeof(ushort)*605*305);
    txtwindow.width=600;
    txtwindow.height=400;
    txtwindow.x=300;
    txtwindow.y=100;
    txtwindow.Title="textedit";
    createwindow(txtwindow);

    wnd pbwindow;
    pbwindow.wndBuffer=(ushort*)malloc(sizeof(ushort)*605*305);
    pbwindow.width=600;
    pbwindow.height=400;
    pbwindow.x=300;
    pbwindow.y=100;
    pbwindow.Title="paintboard";
    createwindow(pbwindow);

    while(1);;
}   