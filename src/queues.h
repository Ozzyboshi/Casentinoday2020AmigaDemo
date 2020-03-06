#ifndef _QUEUES_H_
#define _QUEUES_H_

#include <stdint.h>
#include <stdlib.h>

typedef uint8_t  UBYTE;
typedef uint16_t UWORD;

struct MemPoint
{
    UWORD x;
    UWORD y;
    UBYTE changed;
};

// A structure to represent a queue 
struct Queue 
{ 
	int front, rear; 
	unsigned capacity,size; 
	//struct MemPoint* array; 
	struct MemPoint array[200];
};

inline UBYTE isOverflowed(struct Queue* queue)  { return (UBYTE)(queue->size > queue->capacity); } 

// Queue is full when size becomes equal to the capacity 
inline UBYTE isFull(struct Queue* queue)  { return (UBYTE)(queue->size == queue->capacity); } 

// Queue is empty when size is 0 
inline UBYTE isEmpty(struct Queue* queue) { return (queue->size == 0); } 

struct Queue* createQueue(unsigned);

void enqueue(struct Queue* queue, struct MemPoint item) ;
struct MemPoint dequeue(struct Queue* ) ;


// Function to get front of queue 
struct MemPoint front(struct Queue* ) ;


// Function to get rear of queue 
struct MemPoint rear(struct Queue* ) ;

#endif
