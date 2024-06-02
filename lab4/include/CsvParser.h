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


int isValidCsvRecord(const CsvRecord* record, int column);


int getColumnCountCsvRecordCollection(const CsvRecordCollection* collection);


CsvRecord* getRecordCsvRecordCollection(const CsvRecordCollection* collection, int pos);
void setRecordCsvRecordCollection(CsvRecordCollection* collection, int pos, CsvRecord* record);
char* getFieldCsvRecord(const CsvRecord* record, int pos);


int sizeCsvRecordCollection(const CsvRecordCollection* collection);
int sizeCsvRecord(const CsvRecord* record);


CsvRecord* pushCsvRecord(CsvRecord* record, const char* data);
CsvRecordCollection* pushCsvRecordCollection(CsvRecordCollection* collection, CsvRecord* record);


void* copyCsvRecordCollection(const void* from);
void* copyCsvRecord(const void* from);


CsvRecordCollection* sortCsvRecordCollection(CsvRecordCollection* collection,
                             int column,
                             int (*compar)(const char*, const char*),
                             int (*isValidField)(const char*));


void constructCsvRecordCollection(CsvRecordCollection* collection);
void destructCsvRecordCollection(void* collection, int weakOwnership);


void constructCsvRecord(CsvRecord* record);
void destructCsvRecord(void* record);


#endif // CSVPARSER_H
