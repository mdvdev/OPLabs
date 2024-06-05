#include <stdio.h>
#include <stdlib.h>

#include "List.h"

static List::Node* getNode(const List* list, int pos)
{
    if (!list || list->size <= pos || pos < 0) {
        return NULL;
    }

    List::Node* node = NULL;

    if (pos >= list->size/2) {
        int i = list->size - 1;
        for (List::Node* current = list->last; current; current = current->prev, --i) {
            if (i == pos) {
                node = current;
                break;
            }
        }
    } else {
        int i = 0;
        for (List::Node* current = list->first; current; current = current->next, ++i) {
            if (i == pos) {
                node = current;
                break;
            }
        }
    }

    return node;
}

static void constructNode(List::Node* node, void* data)
{
    if (!node) {
        return;
    }

    node->data = data;
    node->next = node->prev = NULL;
}

static void destructNode(List::Node* node, void (*destructor)(void*))
{
    if (!node || !destructor) {
        return;
    }

    destructor(node->data);
}

List* pushList(List* list, void* data)
{
    if (!list) {
        return NULL;
    }

    List::Node* node = (List::Node*) malloc(sizeof(List::Node));
    if (!node) {
        return NULL;
    }

    constructNode(node, data);

    if (list->last) {
        list->last->next = node;
    } else {
        list->first = node;
    }

    node->next = NULL;
    node->prev = list->last;
    list->last = node;
    list->size++;

    return list;
}

void* popList(List* list)
{
    if (!list) {
        return NULL;
    }

    List::Node* node = list->last;
    node->prev->next = NULL;
    list->last = node->prev;
    list->size--;

    void* data = node->data;

    free(node);

    return data;
}

void constructList(List* list)
{
    if (!list) {
        return;
    }

    list->first = list->last = NULL;
    list->size = 0;
}

void destructList(List* list, void (*destructor)(void*))
{
    if (!list || !destructor) {
        return;
    }

    for (List::Node* current = list->first; current; ) {
        List::Node* tmp = current->next;
        destructNode(current, destructor);
        free(current);
        current = tmp;
    }
}

void* getDataList(const List* list, int pos)
{
    if (!list || list->size <= pos || pos < 0) {
        return NULL;
    }

    List::Node* node = getNode(list, pos);

    return node ? node->data : NULL;
}

void setDataList(List* list, int pos, void* data)
{
    if (!list || list->size <= pos || pos < 0) {
        return;
    }

    List::Node* item = getNode(list, pos);

    if (item) {
        item->data = data;
    }
}

int sizeList(const List* list)
{
    return !list ? 0 : list->size;
}

List* copyList(List* to, const List* from,
               void* (*copyConstructor)(const void*),
               void (*destructor)(void*))
{
    if (!to || !from || !copyConstructor || !destructor) {
        return NULL;
    }

    int fromSize = sizeList(from);

    for (int i = 0; i < fromSize; ++i) {
        void* dataCopy = copyConstructor(getDataList(from, i));
        if (!pushList(to, dataCopy)) {
            destructor(dataCopy);
            return NULL;
        }
    }

    return to;
}
