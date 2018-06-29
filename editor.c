#include "Graphics.h"
#include "user.h"
#include "fcntl.h"
#include "kbd.h"
#include "mouse.h"
#include"character.h"
#include"stat.h"
#include "types.h"

#define EDITOR_WINDOW_WIDTH  600
#define EDITOR_WINDOW_HEIGHT 400
#define EDITOR_WINDOW_POS_X  300
#define EDITOR_WINDOW_POS_Y  100

#define POS_X_ORI            5
#define POS_Y_ORI            25
#define CURSOR_HEIGHT        16
#define CURSOR_WIDTH         1
#define MAX_ROW              20
#define MAX_COL              65

extern wnd EditorWnd;

//光标当前位置
int pos_x = POS_X_ORI;
int pos_y = POS_Y_ORI;
//最后光标行数
int max_y=0;
//每一行字数
int max_line[MAX_ROW]={0};
//文字队列
char letter[MAX_ROW][MAX_COL] = {0};


int getRowFromPosY(int t_pos_y)
{
    return (t_pos_y - POS_Y_ORI) / CHAR_HEIGHT;
}

int getColFromPosX(int t_pos_x)
{
    return (t_pos_x - POS_X_ORI) / CHAR_WIDTH;
}

int getPosXFromCol(int t_col)
{
    return (t_col * CHAR_WIDTH + POS_X_ORI);
}

int getPosYFromRow(int t_row)
{
    return (t_row * CHAR_HEIGHT + POS_Y_ORI);
}

void updateDrawCursor(wnd Window)
{
    draw_rect(Window,pos_x,pos_y,pos_x+CURSOR_WIDTH,pos_y+CURSOR_HEIGHT,RGB(0,0,0),1);
    updateWnd(Window,1);
}

void eraseCursor(wnd Window)
{
     draw_rect(Window,pos_x,pos_y,pos_x+CURSOR_WIDTH,pos_y+CURSOR_HEIGHT,RGB(255,255,255),1);
     updateWnd(Window,1);
}

void appendWord(char ch, int x, int y)
{
    int row, col;
    row = getRowFromPosY(y);
    col = getColFromPosX(x);
    draw_rect(EditorWnd,pos_x,pos_y,pos_x+CHAR_WIDTH,pos_y+CHAR_HEIGHT,RGB(255,255,255),1);
    write_char(EditorWnd,x,y,RGB(0,0,0),RGB(0,0,0),ch);
    if(col == MAX_COL)
    {
        pos_x= POS_X_ORI;
        pos_y+=CHAR_HEIGHT;
    }
    else
    {
       pos_x+=CHAR_WIDTH;
    }        
}

void backspaceWord(int x, int y)
{
    int row, col;
    row = getRowFromPosY(y);
    col = getColFromPosX(x);
    if(col==0&&row!=0)
    {
        pos_x= POS_X_ORI+CHAR_WIDTH*MAX_COL;
        pos_y-=CHAR_HEIGHT;
    }
    else if(col!=0)
   {
       pos_x-=CHAR_WIDTH;
    }      
    draw_rect(EditorWnd,pos_x,pos_y,pos_x+CHAR_WIDTH,pos_y+CHAR_HEIGHT,RGB(255,255,255),1);
}

void deleteword(int x,int y)
{
    int row = getRowFromPosY(y);
    int col = getColFromPosX(x);
    char wtemp,htemp;
    wtemp=letter[row][col-1];
    htemp=wtemp;
    eraseCursor(EditorWnd);
    backspaceWord(pos_x,pos_y);
    int cur_x=pos_x;
    int cur_y=pos_y;
    int i,j;
    i=row;
    j=col;
    for(j = col;j < max_line[row]; j++)
    {
        appendWord(letter[row][j], pos_x,pos_y);
        if((j==max_line[row]-1)&&(row!=max_y))
            letter[row][j]=letter[row+1][0];
        else if((j==max_line[row]-1)&&(row==max_y))
        {
             letter[row][j]=0;
             draw_rect(EditorWnd,pos_x,pos_y,pos_x+CHAR_WIDTH,pos_y+CHAR_HEIGHT,RGB(255,255,255),1);
        }
        else letter[row][j]=letter[row][j+1];
    }

    if(row<max_y)
    {
        for(i = row + 1; i <= max_y; i++)
        {
            for(j = 0; j < max_line[i]; j++)
            {
                 appendWord(letter[i][j], pos_x, pos_y);
                if((j==max_line[i]-1)&&(i!=max_y))
                    letter[i][j]=letter[i+1][0];
                else if((j==max_line[i]-1)&&(i==max_y))
                {
                    letter[i][j]=0;
                    draw_rect(EditorWnd,pos_x,pos_y,pos_x+CHAR_WIDTH,pos_y+CHAR_HEIGHT,RGB(255,255,255),1);
                }
                else letter[i][j]=letter[i][j+1];
            }
        }
    }
    if(getRowFromPosY(pos_y)<max_y) 
    {
        max_line[getRowFromPosY(pos_y)]=MAX_COL;
        max_y--;
    }
    else
    {
        max_line[max_y]--;
    }
    pos_x=cur_x;
    pos_y=cur_y;
    updateDrawCursor(EditorWnd);
}

void insertword(char ch,int x,int y)
{
    int row = getRowFromPosY(y);
    int col = getColFromPosX(x);
    char wtemp = ch;
    char htemp = wtemp;
    eraseCursor(EditorWnd);
    appendWord(ch, pos_x, pos_y);
    int cur_x=pos_x;
    int cur_y=pos_y;
    int i,j;
    i=row;
    j=col;
    for(j = col; j < max_line[row]; j++)
    {
        appendWord(letter[row][j], pos_x,pos_y);
        wtemp=letter[row][j];
        letter[row][j] = htemp;
        htemp=wtemp;
    }
    if(row<max_y)
    {
        for(i = row + 1; i <= max_y; i++)
        {
            for(j = 0; j < max_line[i]; j++)
            {
                 appendWord(letter[i][j], pos_x, pos_y);
                 wtemp=letter[i][j];
                 letter[i][j] = htemp;
                 htemp=wtemp;
            }
        }
    }
    if(getRowFromPosY(pos_y)>max_y) 
    {
        max_line[max_y]=MAX_COL;
        max_y++;
    }
    else 
    {
        max_line[max_y]++;
        letter[i][j]=htemp;
    }
    pos_x=cur_x;
    pos_y=cur_y;
    updateDrawCursor(EditorWnd);
}

void cursorleft(int x,int y)
{
    int row, col;
    int i=0;
    row = getRowFromPosY(y);
    col = getColFromPosX(x);
    eraseCursor(EditorWnd);
    if(col==0 && row!=0)
    {
        pos_y=getPosYFromRow(row-1);
        i=0;
        for(i=MAX_COL-1;i>=0;i--)
        {
            if(letter[row-1][i]!=0)
            {
                pos_x=getPosXFromCol(i);
                break;
            }
        }
    }
   else if(col!=0)
   {
       pos_x-=CHAR_WIDTH;
   }      
   updateDrawCursor(EditorWnd);
}

void cursorright(int x,int y)
{
    int row, col;
    int i=0;
    row = getRowFromPosY(y);
    col = getColFromPosX(x);
    eraseCursor(EditorWnd);
    if((col == max_line[row]) && (row != max_y))
    {
        for(i=0;i<max_line[row+1];i++)
        {
            if(letter[row+1][i]!=0)
            {
                pos_x=getPosXFromCol(i);
                break;
            }
        }
        pos_y=getPosYFromRow(row+1);
    }
    else if((col != max_line[row]))
    {
        pos_x+=CHAR_WIDTH;
    }        
    updateDrawCursor(EditorWnd);
}

void cursorup(int x,int y)
{
    int row, col;
    int i=0;
    row = getRowFromPosY(y);
    col = getColFromPosX(x);
    eraseCursor(EditorWnd);
    if(row!=0)
    {
        for(i=col;i>=0;i--)
        {
             if(letter[row-1][i]!=0)
            {
                pos_x=getPosXFromCol(i);
                break;
            }
        }
        pos_y=getPosYFromRow(row-1);
    }
    updateDrawCursor(EditorWnd);
}

void cursordown(int x,int y)
{
    int row, col;
    int i=0;
    row = getRowFromPosY(y);
    col = getColFromPosX(x);
    eraseCursor(EditorWnd);
    if((row!=max_y))
    {
         for(i=col;i>=0;i--)
        {
             if(letter[row+1][i]!=0)
            {
                pos_x=getPosXFromCol(i);
                break;
            }
        }
        pos_y=getPosYFromRow(row+1);
    }
    updateDrawCursor(EditorWnd);
}

void setup_editor(wnd Window)
{
    pos_x=POS_X_ORI;
    pos_y=POS_Y_ORI;
    updateDrawCursor(Window);
}

void start_editor(wnd Window)
{
    int ch=0;
    while(1)
    {
        ch=kbdgetc();
	    if(ch==KEY_INS)
	    {
 	      ch=0;
	      break;
	    }
    }
    while(1)
    {
        ch=kbdgetc();
        if(ch>=32&&ch<=127)
            insertword(ch,pos_x,pos_y);
        else if(ch==KEY_DEL)
            deleteword(pos_x,pos_y);
	    else if(ch==KEY_LF)
	        cursorleft(pos_x,pos_y);
	    else if(ch==KEY_RT)
	        cursorright(pos_x,pos_y);
        else if(ch==KEY_UP)
            cursorup(pos_x,pos_y);
        else if(ch==KEY_DN)
            cursordown(pos_x,pos_y);
        ch=0;
    } 
}

