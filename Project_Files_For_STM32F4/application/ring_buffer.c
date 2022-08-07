/* Includes ------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "ring_buffer.h"



bool RingBuffer_Init(RingBuffer *ringBuffer, char *dataBuffer, size_t dataBufferSize)
{
    assert(ringBuffer);
    assert(dataBuffer);
    assert(dataBufferSize > 0);

    if ((ringBuffer) && (dataBuffer) && (dataBufferSize > 0)) {
        //TODO
        ringBuffer->head = dataBuffer;
        ringBuffer->tail = dataBuffer;
        ringBuffer->ring_size = dataBufferSize;
        ringBuffer->count = 0;
        ringBuffer->ring_start = dataBuffer;
        return true;
    }

    return false;
}

bool RingBuffer_Clear(RingBuffer *ringBuffer)
{
    assert(ringBuffer);

    if (ringBuffer) {
        //TODO
        char* temp = ringBuffer->head;
        ringBuffer->head = temp;
        ringBuffer->tail = temp;
        ringBuffer->count = 0;
        return true;
    }
    return false;
}

bool RingBuffer_IsEmpty(const RingBuffer *ringBuffer)
{
    assert(ringBuffer);
    //TODO
    if((ringBuffer->head == ringBuffer->tail) && (ringBuffer->count == 0))
        return true;
    else
        return false;
}

size_t RingBuffer_GetLen(const RingBuffer *ringBuffer)
{
    assert(ringBuffer);

    if (ringBuffer) {
        //TODO
        //printf("dlugosc: %d", ringBuffer->count);
        return ringBuffer->count;
    }
    return 0;

}

size_t RingBuffer_GetCapacity(const RingBuffer *ringBuffer)
{
    assert(ringBuffer);

    if (ringBuffer) {
        //TODO
        return ringBuffer->ring_size;
    }
    return 0;
}


bool RingBuffer_PutChar(RingBuffer *ringBuffer, char c)
{
    assert(ringBuffer);

    if (ringBuffer) {
        //printf("PUTCHAR: %d\n",c);
        //TODO
        if(ringBuffer->count >= ringBuffer->ring_size)
            return false;
        if(abs(ringBuffer->ring_start-ringBuffer->head) == ringBuffer->ring_size){
            ringBuffer->head=ringBuffer->ring_start;
            *(ringBuffer->head) = c;
            ringBuffer->head++;
            ringBuffer->count++;
            return true;
        }
        *(ringBuffer->head) = c;
        ringBuffer->head++;
        ringBuffer->count++;
        //printf("glowa: %d\t ogon:%d\t count: %d\n", ringBuffer->head, ringBuffer->tail, ringBuffer->count);
        //printf("glowa-glowa1= %d\n", abs(ringBuffer->ring_start-ringBuffer->head));
        return true;
    }
    return false;
}

bool RingBuffer_GetChar(RingBuffer *ringBuffer, char *c)
{
    assert(ringBuffer);
    assert(c);
    //printf("glowa: %d\n", *ringBuffer->head);
    //printf("ogon: %d\n", *ringBuffer->tail);
    //printf("nasze c: %d\n",*c);
    if ((ringBuffer) && (c)) {
        //TODO
        if((ringBuffer->count > ringBuffer->ring_size) || (ringBuffer->count == 0))
            return false;

        if(abs(ringBuffer->ring_start-ringBuffer->tail) == ringBuffer->ring_size){
            ringBuffer->tail = ringBuffer->ring_start;
            *c = *ringBuffer->tail;
            ringBuffer->tail++;
            ringBuffer->count--;
            return true;
        }

        //printf("GETCHAR: %d\n",*ringBuffer->tail);
        *c = *ringBuffer->tail;
        ringBuffer->tail++;
        ringBuffer->count--;
        //printf("glowa: %d\t ogon:%d\t count: %d\n", ringBuffer->head, ringBuffer->tail, ringBuffer->count);
        //printf("glowa-glowa= %d\n", ringBuffer->head-ringBuffer->ring_start);
        return true;
    }
    return false;
}
