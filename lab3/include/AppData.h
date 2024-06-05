#ifndef APPDATA_H
#define APPDATA_H

#include "MyString.h"
#include "Error.h"

#include "CsvParser.h"

struct AppData {
    CsvRecordCollection* records;
    Error error;
    String errorCount;
    String totalCount;
    String validCount;
    String fileName;
    String region;
    String column;
    String min;
    String max;
    String median;
};

#endif // APPDATA_H
