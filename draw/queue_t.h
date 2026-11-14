#ifndef __TR_QUEUE__
#define __TR_QUEUE__

#include <stdint.h>
#include <malloc.h>

typedef struct node {
    void* data;
    struct node* next;
} node_t;

typedef struct {
    node_t* head;
    node_t* tail;
    uint32_t size;
    uint32_t el_size;
} queue_t;

queue_t* queue_init(uint32_t el_size);
void enqueue(void* data, queue_t* q);
void dequeue(queue_t* q);
void queue_front(queue_t* q, void* data);
void queue_free(queue_t* q);

#endif