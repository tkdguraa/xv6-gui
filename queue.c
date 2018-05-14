#include "queue.h"
#include "types.h"
//a wide use queue
//initialize the queue
void initqueue( queue* que, void* buf, int max, int size)
{
    que->max=max;
    que->size=size;
    que->Array=buf;
    que->putindex=0;
    que->getindex=0;
    que->jud=0;
}

//judge that if queue is full
int JudQueFull(const queue* que)
{
    if(que->putindex==que->getindex&&que->jud==1)
    return 1;
    else
    return 0;
}
//judge that if queue is empty
int JudQueEmpty(const queue* que)
{
    if(que->putindex==que->getindex&&que->jud==0)
    return 1;
    else
    return 0;
}
int PutQueue(queue* que,const void* data)
{
    if(JudQueFull(que)==1)
    return 0;

    memcpy((char*)que->Array+(que->size*que->putindex),data,que->size);
    que->putindex=(que->putindex+1)%que->max;
    que->jud=1;
    return 1;
}
int GetQueue(queue* que,void* data)
{
    if(JudQueEmpty(&que)==1)
    return 0;
   // cprintf("is getqueue working?");
    memcpy(data,(char*)que->Array+(que->size*que->getindex),que->size);
    que->getindex=(que->getindex+1)%que->max;
    que->jud=0;
    return 1;
}