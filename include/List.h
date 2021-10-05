//
// Created by user on 2021/10/5.
//

#ifndef FILECOPYSERVER_LIST_H
#define FILECOPYSERVER_LIST_H

#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"

typedef struct ListNode {
    void *data;
    unsigned int size;
    int index;
    struct ListNode *next;
} ListNode;

typedef struct List {
    ListNode *rear;
    ListNode *front;
    unsigned int size;
} List;

void ListInit(List *list);

void ListDeInit(List *list);

bool IsListEmpty(List *list);

void ListPush(List *list, void *data, unsigned int size, int index);

void *ListPop(List *list, int index);

void ListDelete(List *list, int index);

#endif //FILECOPYSERVER_LIST_H
