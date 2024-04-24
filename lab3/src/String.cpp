#include <stdlib.h>
#include <string.h>

#include "MyString.h"

#define DEFAULT_CAPACITY_SIZE 100

void assignString(String* string, const char* s)
{
    if (!string || !s) {
        return;
    }

    size_t len = strlen(s);
    size_t freeSpace = string->capacity - 1;

    if (len > freeSpace) {
        string->capacity = 2 * (string->capacity + len - freeSpace);
        string->begin= (char*) realloc(string->begin, string->capacity);
    }

    strcpy(string->begin, s);
    string->size = len;
}

void constructString(String* string, const char* s)
{
    if (!string || !s) {
        return;
    }

    size_t len = strlen(s);
    string->capacity = len == 0 ? DEFAULT_CAPACITY_SIZE : 2 * len + 1;
    string->size = len;
    string->begin = (char*) malloc(string->capacity);

    if (string->begin) {
        strcpy(string->begin, s);
    }
}

void destructString(String* string)
{
    if (!string) {
        return;
    }

    free(string->begin);
}
