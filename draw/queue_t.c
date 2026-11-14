#include "queue_t.h"
#include <stdio.h>
#include <memory.h>

node_t* create_node(void* data, uint32_t el_size) {

    node_t* to_insert = (node_t*)malloc(sizeof(node_t));

    if (to_insert == NULL) {
        return NULL;
    }

    to_insert->data = malloc(el_size);

    if (to_insert->data == NULL) {
        free(to_insert);
        return NULL;
    }

    memcpy(to_insert->data, data, el_size);
    to_insert->next = NULL;

    return to_insert;

}

queue_t* queue_init(uint32_t el_size) {

    queue_t* q = (queue_t*)malloc(sizeof(queue_t));

    if (q == NULL) {
        return NULL;
    }

    q->el_size = el_size;
    q->size = 0;
    q->head = q->tail = NULL;

    return q;

}

void enqueue(queue_t* q, void* data) {

    if ((data == NULL) || (q == NULL)) {
        printf("Invalid input data!\n");
        return;
    }

    node_t* to_insert = create_node(data, q->el_size);

    if (to_insert == NULL) {
        printf("Failed to create node!\n");
        return;
    }

    if (q->size == 0) {
        q->head = q->tail = to_insert;
    }
    else {
        q->tail->next = to_insert;
        q->tail = to_insert;
    }

    q->size++;

}

void dequeue(queue_t* q) {
    if (q == NULL) {
        printf("Invalid input data!\n");
        return;
    }

    if (q->size == 0) {
        return;
    }

    node_t* to_del = q->head;

    if (q->size == 1) {
        free(to_del->data);
        free(to_del);
        q->head = q->tail = NULL;
        q->size--;
        return;
    }

    q->head = q->head->next;
    free(to_del->data);
    free(to_del);
    q->size--;

}

void queue_front(queue_t* q, void* data) {
    if ((q == NULL) || (data == NULL)) {
        printf("Invalid input data!\n");
        return;
    }

    if (q->size == 0) {
        return;
    }

    memcpy(data, q->head->data, q->el_size);

}

void queue_free(queue_t* q) {
    
    node_t* current = q->head;

    while(current != NULL) {
        node_t* next = current->next;
        free(current->data);
        free(current);
        current = next;
    }

    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    q->el_size = 0;

    free(q);

}
