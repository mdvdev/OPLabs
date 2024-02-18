#include <stdlib.h>
#include <string.h>

#include "charvector.h"

#define DEFAULT_CAPACITY_SIZE 100

void assignCharVector(CharVector* charVector, const char* string)
{
    size_t len = strlen(string);
    if (len > charVector->capacity) {
        charVector->capacity = 2 * (charVector->capacity + len);
        charVector->begin= (char*) realloc(charVector->begin, charVector->capacity);
    }
    strcpy(charVector->begin, string);
    charVector->size = len;
}

CharVector* constructCharVector(CharVector* charVector, const char* string)
{
    size_t len = strlen(string);
    charVector->capacity = len == 0 ? DEFAULT_CAPACITY_SIZE : 2 * len;
    charVector->size = len;
    charVector->begin = (char*) malloc(charVector->capacity);
    if (charVector->begin == NULL) {
        return NULL;
    }
    strcpy(charVector->begin, string);
    return charVector;
}

void destructCharVector(CharVector* charVector)
{
    free(charVector->begin);
}
