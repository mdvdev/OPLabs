#include <stdio.h>
#include <stdlib.h>

#include "List.h"

ListItem* constructListItem(void* data)
{
    ListItem* item = (ListItem*) malloc(sizeof(ListItem));
    if (!item) {
        return NULL;
    }

    item->data = data;
    item->next = item->prev = NULL;

    return item;
}

ListItem* pushList(List* list, ListItem* item)
{
    if (!list || !item) {
        return NULL;
    }

    if (list->last) {
        list->last->next = item;
    } else {
        list->first = item;
    }

    item->next = NULL;
    item->prev = list->last;
    list->last = item;
    list->size++;

    return item;
}

ListItem* popList(List* list)
{
    if (!list) {
        return NULL;
    }

    ListItem* item = list->last;
    item->prev->next = NULL;
    list->last = item->prev;
    list->size--;

    return item;
}

List createList()
{
    List list = {
        .first = NULL,
        .last = NULL,
        .size = 0
    };

    return list;
}

void freeList(List* list, void (*destructor)(void*))
{
    if (!list || !destructor) {
        return;
    }

    for (ListItem* current = list->first; current; ) {
        ListItem* tmp = current->next;
        destructListItem(current, destructor);
        current = tmp;
    }
}

void* getDataList(const List* list, size_t pos)
{
    if (!list || list->size <= pos) {
        return NULL;
    }

    void* data = NULL;

    if (pos >= list->size/2) {
        size_t i = list->size - 1;
        for (const ListItem* current = list->last; current; current = current->prev, --i) {
            if (i == pos) {
                data = current->data;
                break;
            }
        }
    } else {
        size_t i = 0;
        for (const ListItem* current = list->first; current; current = current->next, ++i) {
            if (i == pos) {
                data = current->data;
                break;
            }
        }
    }

    return data;
}

void destructListItem(ListItem* item, void (*destructor)(void*))
{
    if (!item) {
        return;
    }
    destructor(item->data);
    free(item);
}

size_t sizeList(const List* list)
{
    return list->size;
}
