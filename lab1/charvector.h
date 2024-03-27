#ifndef CHARVECTOR_H
#define CHARVECTOR_H

#include <stdlib.h>

struct CharVector {
    char* begin;
    size_t size;
    size_t capacity;
};

CharVector* constructCharVector(CharVector* charVector, const char* string);
void destructCharVector(CharVector* charVector);
void assignCharVector(CharVector* charVector, const char* string);

#endif // CHARVECTOR_H
