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
    int jud;
}queue;
void initqueue( queue* que, void* buf, int max, int size);
int JudQueFull(const queue* que);
int JudQueEmpty(const queue* que);
int PutQueue(queue* que,const void* data);
int GetQueue(queue* que,void* data);
void* memcpy(void *dst, const void *src, uint n);
#endif