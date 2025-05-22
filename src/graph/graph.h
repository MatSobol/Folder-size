#ifndef GRAPH_H
#define GRAPH_H

#include <windows.h>

typedef struct Node
{
    ULONGLONG size;
    char *path;
    int capacity;
    int childNum;
    struct Node **children;
} Node;

Node *init(char *path);
void addChild(Node *node, Node *child);
void sumChildren(Node *node);
void freeNodes(Node *node);
void printGraph(Node *root);
Node *getFreeNode(Node *node);

#endif