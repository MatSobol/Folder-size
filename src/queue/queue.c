#include <stdio.h>

#include "queue.h"
#include "../graph/graph.h"
#include "windows.h"

void initQueue(Queue* q) {
    q->root = NULL;
    InitializeCriticalSection(&q->cs);
}

void addQueue(Queue* q, Node *value){
    Element* newElement = (Element*)malloc(sizeof(Element));
    newElement->value = value;
    newElement->next = q->root;
    q->root = newElement;
}

Node *popQueue(Queue* q){
    EnterCriticalSection(&q->cs); 
    if(q->root == NULL){
        LeaveCriticalSection(&q->cs);
        return NULL;
    }
    Element *temp = q->root;
    Node *tempValue = temp->value;
    q->root = q->root->next;
    LeaveCriticalSection(&q->cs);
    free(temp);
    return tempValue;
}

void destroyQueue(Queue* q) {
    while (popQueue(q) != NULL);
    DeleteCriticalSection(&q->cs);
}