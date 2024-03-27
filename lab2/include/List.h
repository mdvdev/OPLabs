#ifndef LIST_H
#define LIST_H

struct List {
    struct Node {
        void* data;
        Node* next;
        Node* prev;
    };

    Node* first;
    Node* last;
    int size;
};


void constructList(List* list);
void destructList(List* list, void (*destructor)(void* data));


List* copyList(List* to, const List* from,
               void* (*copyConstructor)(const void*),
               void (*destructor)(void*));


List* pushList(List* list, void* data);
void* popList(List* list);


void* getDataList(const List* list, int pos);
void setDataList(List* list, int pos, void* data);


int sizeList(const List* list);

#endif // LIST_H
