#include <stdio.h>

#include "graph.h"
#include <windows.h>

#define DEFAULT_CAPACITY 32

Node *init(char *path)
{
    Node *node = malloc(sizeof(Node));
    node->size = 0;
    node->path = _strdup(path);
    node->capacity = DEFAULT_CAPACITY;
    node->childNum = 0;
    node->children = malloc(sizeof(Node *) * DEFAULT_CAPACITY);
    return node;
}

void addChild(Node *node, Node *child)
{
    if (node->childNum >= node->capacity)
    {
        int newCapacity = node->capacity == 0 ? 2 : node->capacity * 2;
        node->children = realloc(node->children, newCapacity * sizeof(Node *));
        node->capacity = newCapacity;
    }
    node->children[node->childNum++] = child;
}

void sumChildren(Node *node)
{
    for (int i = 0; i < node->childNum; i++)
    {
        node->size += node->children[i]->size;
    }
}

void freeNodes(Node *node)
{
    for (int i = 0; i < node->childNum; i++)
    {
        freeNodes(node->children[i]);
    }
    free(node->children);
    free(node);
}

void printGraph(Node *root)
{
    printf("%s\n", root->path);
    for (int i = 0; i < root->childNum; i++)
    {
        printGraph(root->children[i]);
    }
}
