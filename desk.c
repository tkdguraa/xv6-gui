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
    PICNODE IMGicon;
    loadBitmap(&bgPic, "desk.bmp"); 
    set_icon_alpha(&bgPic);
    loadimg(bgPic,0,0,0,0);

    loadBitmap(&textedit,"textedit.bmp");
    set_icon_alpha(&textedit);
    loadimg(textedit,350,550,350,550);

    loadBitmap(&paintboard,"paintboard.bmp");
    set_icon_alpha(&paintboard);
    loadimg(paintboard,470,550,470,550);

    loadBitmap(&IMGicon,"test.bmp");
    set_icon_alpha(&IMGicon);
    loadimg(IMGicon,590,550,590,550);

    wnd Editorwindow;
    Editorwindow.wndBuffer=(ushort*)malloc(sizeof(ushort)*605*405);
    Editorwindow.width=600;
    Editorwindow.height=400;
    Editorwindow.x=150;
    Editorwindow.y=100;
    Editorwindow.Title="TextEdit";
    createwindow(Editorwindow);


    wnd pbwindow;
    pbwindow.wndBuffer=(ushort*)malloc(sizeof(ushort)*605*405);
    pbwindow.width=600;
    pbwindow.height=400;
    pbwindow.x=300;
    pbwindow.y=100;
    pbwindow.Title="paintboard";
    createwindow(pbwindow);

    wnd test;
    test.wndBuffer=(ushort*)malloc(sizeof(ushort)*605*405);
    test.width=600;
    test.height=400;
    test.x=400;
    test.y=150;
    test.Title="Test";
    createwindow(test);

    while(1);;
}   
