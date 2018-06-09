//a wide use queue
#ifndef QUEUE_H
#define QUEUE_H
#include "types.h"
typedef struct queue
{
    int size;
    int max;
    void* Array;
    int putindex;
    int getindex;
    BOOL jud;
}queue;
void initqueue( queue* que, void* buf, int max, int size);
BOOL JudQueFull(const queue* que);
BOOL JudQueEmpty(const queue* que);
BOOL PutQueue(queue* que,const void* data);
BOOL GetQueue(queue* que,void* data);
void* memcpy(void *dst, const void *src, uint n);
#endif