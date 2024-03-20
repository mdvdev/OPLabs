#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <stdio.h>

#include "List.h"

struct CsvRecord {
    List items;
};

struct CsvRecordCollection {
    List records;
};


CsvRecordCollection* parseCsv(FILE* fp);


const CsvRecord* getRecordCsvRecordCollection(const CsvRecordCollection* collection, size_t pos);
const char* getFieldCsvRecord(const CsvRecord* record, size_t pos);


size_t sizeCsvRecordCollection(const CsvRecordCollection* collection);
size_t sizeCsvRecord(const CsvRecord* collection);


CsvRecord* pushCsvRecord(CsvRecord* record, void* data);
CsvRecordCollection* pushCsvRecordCollection(CsvRecordCollection* collection, CsvRecord* record);


CsvRecordCollection* constructCollection();
void destructCollection(void* collection);


CsvRecord* constructRecord();
void destructRecord(void* record);


#endif // CSVPARSER_H
