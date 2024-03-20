#ifndef APPDATA_H
#define APPDATA_H

#include "String.h"
#include "Error.h"

#include "CsvParser.h"

struct AppData {
    CsvRecordCollection* records;
    Error error;
    String fileName;
    String region;
    size_t column;
    size_t min;
    size_t max;
    size_t median;
};

#endif // APPDATA_H
