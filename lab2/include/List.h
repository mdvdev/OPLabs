#ifndef LIST_H
#define LIST_H

#include <stdio.h>

struct ListItem {
    void* data;
    ListItem* next;
    ListItem* prev;
};

struct List {
    ListItem* first;
    ListItem* last;
    size_t size;
};


List createList();
void freeList(List* list, void (*destructor)(void* data));


ListItem* constructListItem(void* data);
void destructListItem(ListItem* item, void (*destructor)(void* data));


ListItem* pushList(List* list, ListItem* item);
ListItem* popList(List* list);


void* getDataList(const List* list, size_t pos);


size_t sizeList(const List* list);

#endif // LIST_H
