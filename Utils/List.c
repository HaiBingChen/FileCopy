//
// Created by user on 2021/10/5.
//

#include "List.h"
#include "assert.h"
#include "memory.h"

const unsigned int cMaxListSize = 30;

void ListInit(List *list) {
    assert(list != NULL);

    list->front = NULL;
    list->rear = NULL;
    list->size = 0;
}

void ListDeInit(List *list) {
    assert(list != NULL);
    ListNode *node = NULL;
    while (list->front) {
        node = list->front;
        list->front = list->front->next;
        free(node->data);
        free(node);
    }
}

bool IsListEmpty(List *list) {
    assert(list != NULL);

    return (list->size == 0);
}

void ListPush(List *list, void *data, unsigned int size, int index) {
    assert(list != NULL);
    assert(data != NULL);

    if (list->size > cMaxListSize) {
        printf("[ERROR] %s: list is overflow\n", __FUNCTION__);
        return;
    }

    ListNode *node = (ListNode *) malloc(sizeof(ListNode));
    node->next = NULL;
    node->index = index;
    node->size = size;
    node->data = malloc(size);
    memcpy(node->data, data, size);

    if (IsListEmpty(list)) {
        list->front = node;
        list->rear = node;
    } else {
        list->rear->next = node;
        list->rear = node;
    }

    ++list->size;
}

void *ListPop(List *list, int index) {
    assert(list != NULL);

    if (IsListEmpty(list)) {
        return NULL;
    }

    ListNode *node = list->front;

    do {
        if (node->index == index) {
            break;
        }

        node = node->next;

        if (node == NULL) {
            printf("%s: can't find index %d\n", __FUNCTION__, index);
            break;
        }
    } while (1);

    return node->data;
}

void ListDelete(List *list, int index) {
    assert(list != NULL);

    if (IsListEmpty(list)) {
        return;
    }

    ListNode *node = list->front;

    //链表头，第一个元素匹配
    if (node->index == index) {
        list->front = list->front->next;
        //只有一个元素，删掉后头尾指针都要指向空
        if (list->front == NULL) {
            list->rear = NULL;
        }

        free(node->data);
        free(node);
        node = NULL;
        --list->size;
        return;
    }

    do {
        ListNode *node_last = node;
        node = node->next;

        //第一个元素不匹配，第二个元素为空，则说明只有一个元素，结束操作
        if (node == NULL) {
            printf("%s: can't find index %d\n", __FUNCTION__, index);
            break;
        }

        //第n个元素匹配(n>=2)
        if (node->index == index) {
            //链表尾删除
            if (node == list->rear) {
                //尾指针指向上一个元素
                list->rear = node_last;
                free(node->data);
                free(node);
                node = NULL;
                --list->size;
            } else {
                node_last->next = node->next;
                free(node->data);
                free(node);
                node = NULL;
                --list->size;
            }

            break;
        }
    } while (1);
}
