#ifndef QUEUE_H
#define QUEUE_H

#include "../graph/graph.h"

typedef struct Element {
    Node* value;
    struct Element* next;
} Element;

typedef struct {
    Element* root;
    CRITICAL_SECTION cs;
} Queue;

void initQueue(Queue* q);
void addQueue(Queue* q, Node *value);
Node *popQueue(Queue* q);
void destroyQueue(Queue* q);


#endif