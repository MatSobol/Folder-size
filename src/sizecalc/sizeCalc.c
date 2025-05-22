#include <stdio.h>
#include <strsafe.h>

#include "../graph/graph.h"
#include "../queue/queue.h"
#include "../sizecalc/sizeCalc.h"
#include "../sqlite/sqlite.h"

#define MAX_THREAD_NUM 4

void GetDirectorySize(Node *current)
{
    char *path = current->path;
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind;
    const int pathSize = strlen(path);
    char *searchPath = malloc(sizeof(char) * pathSize + 3);

    StringCchPrintfA(searchPath, pathSize + 3, "%s\\*", path);

    hFind = FindFirstFileA(searchPath, &findFileData);
    free(searchPath);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        return;
    }

    do
    {
        if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0)
        {
            int fileSize = strlen(findFileData.cFileName);
            char *fullPath = malloc(sizeof(char) * (pathSize + fileSize + 2));
            StringCchPrintfA(fullPath, pathSize + fileSize + 2, "%s\\%s", path, findFileData.cFileName);
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                Node *child = init(fullPath);
                addChild(current, child);
                GetDirectorySize(child);
                free(fullPath);
            }
            else
            {
                ULARGE_INTEGER fileSize;
                fileSize.LowPart = findFileData.nFileSizeLow;
                fileSize.HighPart = findFileData.nFileSizeHigh;
                current->size += fileSize.QuadPart;
            }
        }
    } while (FindNextFileA(hFind, &findFileData) != 0);

    FindClose(hFind);

    sumChildren(current);

    DWORD dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES)
    {
        printf("Error enumerating directory: %s (Error code: %lu)\n", path, dwError);
    }
}

DWORD WINAPI DirectorySizeThread(LPVOID lpParam)
{
    Queue *q = (Queue *)lpParam;
    Node *val;
    while ((val = popQueue(q)) != NULL)
    {
        GetDirectorySize(val);
    }
    return 0;
}

void printResult(ULONGLONG folderSize)
{
    if (folderSize >= 0)
    {
        printf("Size in B: %.2f\n", (double)folderSize);
        double sizeInKB = (double)folderSize / 1024.0;
        double sizeInMB = sizeInKB / 1024.0;
        if (sizeInMB > 200.0)
        {
            double sizeInGB = sizeInMB / 1024.0;
            if (sizeInGB > 2.0)
            {
                printf("Size in GB: %.2f\n", sizeInGB);
                return;
            }
            printf("Size in MB: %.2f\n", sizeInMB);
            return;
        }
        printf("Size in KB: %.2f\n", sizeInKB);
    }
    else
    {
        printf("Could not get the size of folder or the folder is empty.\n");
    }
}

void initNode(Node *node)
{
    char *path = node->path;
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind;
    const int pathSize = strlen(path);
    char *searchPath = malloc(sizeof(char) * pathSize + 3);

    StringCchPrintfA(searchPath, pathSize + 3, "%s\\*", path);

    hFind = FindFirstFileA(searchPath, &findFileData);
    free(searchPath);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        return;
    }

    do
    {
        if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0)
        {
            int fileSize = strlen(findFileData.cFileName);
            char *fullPath = malloc(sizeof(char) * (pathSize + fileSize + 2));
            StringCchPrintfA(fullPath, pathSize + fileSize + 2, "%s\\%s", path, findFileData.cFileName);
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                Node *child = init(fullPath);
                addChild(node, child);
                free(fullPath);
            }
            else
            {
                ULARGE_INTEGER fileSize;
                fileSize.LowPart = findFileData.nFileSizeLow;
                fileSize.HighPart = findFileData.nFileSizeHigh;
                node->size += fileSize.QuadPart;
            }
        }
    } while (FindNextFileA(hFind, &findFileData) != 0);
}

void MeasureSize(char *folderPath)
{
    Node *root = init(folderPath);
    initNode(root);

    Queue *q = (Queue *)malloc(sizeof(Queue));
    initQueue(q);

    for (int i = 0; i < root->childNum; i++)
    {
        initNode(root->children[i]);
        for (int j = 0; j < root->children[i]->childNum; j++)
        {
            addQueue(q, root->children[i]->children[j]);
        }
    }

    HANDLE hThread[MAX_THREAD_NUM];

    for (int i = 0; i < MAX_THREAD_NUM; i++)
    {
        hThread[i] = CreateThread(
            NULL,
            0,
            DirectorySizeThread,
            q,
            0,
            NULL);

        if (hThread[i] == NULL)
        {
            printf("Failed to create thread. Error: %lu\n", GetLastError());
            return;
        }
    }
    WaitForMultipleObjects(MAX_THREAD_NUM, hThread, TRUE, INFINITE);
    for (int i = 0; i < MAX_THREAD_NUM; i++)
    {
        CloseHandle(hThread[i]);
    }
    for (int i = 0; i < root->childNum; i++)
    {
        for (int j = 0; j < root->children[i]->childNum; j++)
        {
            root->children[i]->size += root->children[i]->children[j]->size;
        }
        root->size += root->children[i]->size;
    }

    printResult(root->size);
    save_graph(root);
    freeNodes(root);
    destroyQueue(q);
}