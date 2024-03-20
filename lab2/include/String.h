#ifndef STRING_H
#define STRING_H

#include <stdlib.h>

struct String {
    char* begin;
    size_t size;
    size_t capacity;
};

String* initString(String* string, const char* s);
void destructString(String* string);
void assignString(String* string, const char* s);

#endif // STRING_H
