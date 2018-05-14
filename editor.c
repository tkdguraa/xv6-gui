#include "types.h"
#include "gui_base.h"
#include "user.h"
#include "fcntl.h"
#include "msg.h"
#include "xv6_api.h"
#include "kbd.h"

#define EDITOR_WINDOW_WIDTH  400
#define EDITOR_WINDOW_HEIGHT 300
#define EDITOR_WINDOW_POS_X  200
#define EDITOR_WINDOW_POS_Y  150

#define POS_X_ORI            0
#define POS_Y_ORI            0
#define CHAR_WIDTH           10
#define CHAR_HEIGHT          20
#define CURSOR_HEIGHT        20
#define CURSOR_WIDTH         1
#define MAX_ROW              15
#define MAX_COL              39

Window wnd;
int pos_x = POS_X_ORI;
int pos_y = POS_Y_ORI;
RGBA RGBA_black={255,0,0,0};
RGB RGB_white={255,255,255};
RGB RGB_black = {0, 0, 0};
RGB *background;
char null[2]="  ";
char letter[MAX_ROW][MAX_COL] = {0};
int leftShift = 0;

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

// add a word at this position
// without moving its coordinates
void appendWord(char ch, int x, int y)
{
    int row, col;
    row = getRowFromPosY(y);
    col = getColFromPosX(x);
    letter[row][col] = ch;
    api_drawRect(&wnd,(Point){x, y},(Size){CHAR_HEIGHT, CHAR_WIDTH}, RGB_white);
    api_drawCharacter(&wnd,x, y,ch, RGBA_black);
    api_update(&wnd,(Rect){x, y,CHAR_WIDTH,CHAR_HEIGHT});
}

// delete a word without moving its coordinates
void backspaceWord(int x, int y)
{
    int row, col;
    row = getRowFromPosY(y);
    col = getColFromPosX(x);
    letter[row][col] = 0;
    api_drawRect(&wnd,(Point){x, y},(Size){CHAR_HEIGHT, CHAR_WIDTH}, RGB_white);
    api_update(&wnd,(Rect){x, y, CHAR_WIDTH,CHAR_HEIGHT});
}

// insert in present position
void insertWord(char ch, int x, int y)
{
    int row = getRowFromPosY(y);
    int col = getColFromPosX(x);

    char wtemp = letter[row][col];
    char rtemp = wtemp;
    appendWord(ch, x, y);

    int i, j;

    
    while(wtemp)
    {
        if(col == MAX_COL)
        {
            col = 0;
            ++row;
        }
        else
        {
            ++col;
        }
        rtemp = letter[row][col];
        x = getPosXFromCol(col);
        y = getPosYFromRow(row);
        letter[row][col] = wtemp;
        wtemp = rtemp;
    }
    for(i = 0; i < MAX_ROW; ++i)
    {
        for(j = 0; j < MAX_COL; ++j)
        {
            if(!(letter[i][i]))
            {
                break;
            }
            appendWord(letter[i][j], getPosXFromCol(j), getPosYFromRow(i));
        }
        if(j < MAX_COL - 1)
            break;
    }
}
void deleteWord(int x, int y)
{

    int row = getRowFromPosY(y);
    int col = getColFromPosX(x);
        backspaceWord(x, y);
    
    int i, j;

    do
    {
        if(col == MAX_COL)
        {
            col = 0;
            ++row;
        }
        else
        {
            ++col;
        }

        letter[(col == 0) ? (row - 1) : row][(col == 0) ? MAX_COL : (col - 1)] = letter[row][col];
    }while(letter[row][col]);

    for(i = 0; i < MAX_ROW; ++i)
    {
        for(j = 0; j < MAX_COL; ++j)
        {
            if(!(letter[i][i]))
            {
                break;
            }
            appendWord(letter[i][j], getPosXFromCol(j), getPosYFromRow(i));
        }
        if(j < MAX_COL - 1)
            break;
    }
}


void updateDrawCursor()
{
    api_drawRect(&wnd, (Point){pos_x, pos_y}, (Size){CURSOR_HEIGHT, CURSOR_WIDTH}, RGB_black);
    api_update(&wnd,(Rect){pos_x,pos_y,CURSOR_WIDTH,CURSOR_HEIGHT});
}

void eraseCursor()
{
    api_drawRect(&wnd, (Point){pos_x, pos_y}, (Size){CURSOR_HEIGHT, CURSOR_WIDTH}, RGB_white);
    api_update(&wnd,(Rect){pos_x,pos_y,CURSOR_WIDTH,CURSOR_HEIGHT});
}

void MsgProc(struct message * msg)
{

    eraseCursor();
    switch(msg->msg_type)
    {
    case M_KEY_DOWN:
        switch(msg->params[0])
        {
            // backspace
        case 8:
            if(!pos_x)
            {
                if(pos_y == POS_Y_ORI)
                {
                    break;
                }
                // appears in the first column
                else
                {
                    pos_y -= CHAR_HEIGHT;
                    pos_x = EDITOR_WINDOW_WIDTH - 2 * CHAR_WIDTH;
                }
            }
            else
            {
                pos_x -= CHAR_WIDTH;
            }
            if(letter[0][0])
            {
                if(leftShift == 0)
                    backspaceWord(pos_x, pos_y);
                else
                {
                    deleteWord(pos_x, pos_y);
                }
            }
            break;

            // up
        case 226:
            break;

            // down
        case 227:
            break;

            // left
        case 228:
            if(!pos_x)
            {
                if(pos_y == POS_Y_ORI)
                {
                    break;
                }
                // appears in the first column
                else
                {
                    pos_y -= CHAR_HEIGHT;
                    pos_x = EDITOR_WINDOW_WIDTH - 2 * CHAR_WIDTH;
                    --leftShift;
                }
            }
            else
            {
                pos_x -= CHAR_WIDTH;
                -- leftShift;
            }
            break;

            // right
        case 229:
            if(leftShift == 0)
            {
                break;
            }
            if(pos_x == EDITOR_WINDOW_WIDTH - 2 * CHAR_WIDTH)
            {
                if(pos_y == POS_Y_ORI + CHAR_HEIGHT * MAX_ROW)
                {
                    break;
                }
                // appears in the last column
                else
                {
                    pos_y += CHAR_HEIGHT;
                    pos_x = POS_X_ORI;
                    if(leftShift < 0)
                    {
                        ++leftShift;
                    }
                }
            }
            else
            {
                pos_x += CHAR_WIDTH;
                if(leftShift < 0)
                {
                    ++leftShift;
                }
            }
            break;

            // letters
        default:
            if(msg->params[0] > 31)
            {
                if(leftShift == 0)
                {
                    appendWord((msg->params[1] == 1) ? (char)(msg->params[0]-32) : (char)(msg->params[0]), pos_x, pos_y);
                }
                else
                {
                    insertWord((msg->params[1] == 1) ? (char)(msg->params[0]-32) : (char)(msg->params[0]), pos_x, pos_y);
                }

                if(pos_x == EDITOR_WINDOW_WIDTH - 2 * CHAR_WIDTH)
                {
                    if(pos_y == POS_Y_ORI + CHAR_HEIGHT * MAX_ROW)
                    {
                        break;
                    }
                // appears in the last column
                    else
                    {
                        pos_y += CHAR_HEIGHT;
                        pos_x = POS_X_ORI;
                    }
                }
                else
                {
                    pos_x += CHAR_WIDTH;
                }
            }
            break;
        }
        break;

    case M_KEY_UP:
        break;

    case M_MOUSE_LEFT_CLICK:
        break;
    case M_CLOSE_WINDOW:
        free(background);
        api_destroywindow(&wnd);
        break;
    }

    if(msg->msg_type != M_CLOSE_WINDOW)
    updateDrawCursor();
}


int main(void)
{
    wnd.pos.x = EDITOR_WINDOW_POS_X;
    wnd.pos.y = EDITOR_WINDOW_POS_Y;
    wnd.size.w = EDITOR_WINDOW_WIDTH;
    wnd.size.h = EDITOR_WINDOW_HEIGHT;
    wnd.title = "TextEditor";
    api_createwindow(&wnd);
    background = malloc(EDITOR_WINDOW_WIDTH * EDITOR_WINDOW_HEIGHT * 3);
    int i = 0;
    for (i = 0; i < EDITOR_WINDOW_WIDTH * EDITOR_WINDOW_HEIGHT;i++)
    {
        //background[i].R = 255;
        //background[i].G = 255;
        //background[i].B = 255;
        background[i] = RGB_white;
    }
    api_paint24BitmapToContent(&wnd, background, (Point){0,0}, (Point){0,0},
                            (Size){EDITOR_WINDOW_HEIGHT, EDITOR_WINDOW_WIDTH},
                            (Size){EDITOR_WINDOW_HEIGHT, EDITOR_WINDOW_WIDTH});
    api_repaint(&wnd);

    updateDrawCursor();
    api_exec(&wnd,&MsgProc);
    
    
    return 0;
}
