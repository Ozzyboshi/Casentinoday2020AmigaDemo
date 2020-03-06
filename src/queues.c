#include "queues.h"

static struct MemPoint point_empty={0,0,0};

// function to create a queue of given capacity. 
// It initializes size of queue as 0 
struct Queue* createQueue(unsigned capacity) 
{ 
	struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue)); 
	queue->capacity = capacity; 
	queue->front = queue->size = 0; 
	queue->rear = capacity - 1; // This is important, see the enqueue 
	//queue->array = (struct MemPoint*) malloc(queue->capacity * sizeof(struct MemPoint)); 
	return queue; 
} 

// Function to add an item to the queue. 
// It changes rear and size 
void enqueue(struct Queue* queue, struct MemPoint item) 
{ 
	if (isFull(queue)) 
		return; 
	queue->rear = (queue->rear + 1)%queue->capacity; 
	if (queue->rear<200)
		queue->array[queue->rear] = item; 
	queue->size = queue->size + 1; 
} 

// Function to remove an item from queue. 
// It changes front and size 
struct MemPoint dequeue(struct Queue* queue) 
{ 
	if (isEmpty(queue)) 
		return point_empty; 
	struct MemPoint item = queue->array[queue->front]; 
	queue->front = (queue->front + 1)%queue->capacity; 
	queue->size = queue->size - 1; 
	return item; 
} 

// Function to get front of queue 
struct MemPoint front(struct Queue* queue) 
{ 
	if (isEmpty(queue)) 
		return point_empty; 
	return queue->array[queue->front]; 
} 

// Function to get rear of queue 
struct MemPoint rear(struct Queue* queue) 
{ 

	if (isEmpty(queue)) 
		return point_empty; 
	return queue->array[queue->rear]; 
} 

