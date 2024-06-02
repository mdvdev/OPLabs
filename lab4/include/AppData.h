#ifndef APPDATA_H
#define APPDATA_H

#include "MyString.h"
#include "Error.h"

struct List;

struct AppData {
    List* pointMatrix;
    Error error;
    String normalizeFrom;
    String normalizeTo;
    String errorCount;
    String totalCount;
    String validCount;
    String fileName;
};

#endif // APPDATA_H
