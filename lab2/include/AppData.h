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
    String column;
    String min;
    String max;
    String median;
};

#endif // APPDATA_H
