#include <stdlib.h>
#include <string.h>

#include "CsvParser.h"

static const char* processDelimeter(const char* begin, CsvRecord* record)
{
    if (!begin || !record || *begin != ',') {
        return NULL;
    }

    if (!pushCsvRecord(record, NULL)) {
        return NULL;
    }

    begin++;

    if (*begin == '\0') {
        if (!pushCsvRecord(record, NULL)) {
            return NULL;
        }
    }

    return begin;
}

static const char* processToken(const char* begin, CsvRecord* record)
{
    if (!begin || !record || *begin == ',') {
        return NULL;
    }

    const char* end;
    for (end = begin; *end && *end != ','; ++end)
        ;

    char* str = strndup(begin, end - begin);
    if (!str) {
        return NULL;
    }

    if (!pushCsvRecord(record, str)) {
        free(str);
        return NULL;
    }

    if (*end && *(end + 1) == '\0') {
        if (!pushCsvRecord(record, NULL)) {
            return NULL;
        }
    }

    begin = *end == ',' ? end + 1 : end;

    return begin;
}

static CsvRecord* stringToCsvRecord(const char* str)
{
    if (!str) {
        return NULL;
    }

    CsvRecord* record = (CsvRecord*) malloc(sizeof(CsvRecord));
    if (!record) {
        return NULL;
    }

    constructCsvRecord(record);

    const char* pos = str;

    while (pos && *pos) {
        pos = *pos == ',' ?
                processDelimeter(pos, record) :
                processToken(pos, record);
    }

    if (!pos) {
        destructCsvRecord(record);
        free(record);
        record = NULL;
    }

    return record;
}

static CsvRecordCollection* processString(const char* str, CsvRecordCollection* collection)
{
    if (!str || !collection) {
        return NULL;
    }

    CsvRecord* record = stringToCsvRecord(str);
    if (!record) {
        return NULL;
    }

    if (!pushCsvRecordCollection(collection, record)) {
        destructCsvRecord(record);
        free(record);
        return NULL;
    }

    return collection;
}

static void* dupString(const void* str)
{
    if (!str) {
        return NULL;
    }

    const char* str_ = (const char*) str;

    return strdup(str_);
}

int isValidCsvRecord(const CsvRecord* record, int column)
{
    if (!record || column <= 0) {
        return 0;
    }
    return sizeCsvRecord(record) == column;
}

CsvRecordCollection* parseCsv(FILE* fp)
{
    if (!fp) {
        return NULL;
    }

    CsvRecordCollection* collection = (CsvRecordCollection*) malloc(sizeof(CsvRecordCollection));
    if (!collection) {
        return NULL;
    }

    constructCsvRecordCollection(collection);

    char* buf = NULL;
    ssize_t rc;
    size_t num;

    while ((rc = getline(&buf, &num, fp)) != -1) {
        buf[rc-1] = '\0';
        if (!processString(buf, collection)) {
            destructCsvRecordCollection(collection);
            free(collection);
            collection = NULL;
            break;
        }
    }

    free(buf);

    if (ferror(fp)) {
        destructCsvRecordCollection(collection);
        free(collection);
        collection = NULL;
    }

    return collection;
}

void constructCsvRecordCollection(CsvRecordCollection* collection)
{
    if (!collection) {
        return;
    }

    constructList(&collection->records);
}

void destructCsvRecordCollection(void* collection)
{
    if (!collection) {
        return;
    }

    CsvRecordCollection* collection_ = (CsvRecordCollection*) collection;
    destructList(&collection_->records, destructCsvRecord);
}

void constructCsvRecord(CsvRecord* record)
{
    if (!record) {
        return;
    }

    constructList(&record->items);
}

void destructCsvRecord(void* record)
{
    if (!record) {
        return;
    }

    CsvRecord* record_ = (CsvRecord*) record;
    destructList(&record_->items, free);
}

CsvRecord* pushCsvRecord(CsvRecord* record, void* data)
{
    if (!record) {
        return NULL;
    }

    if (!pushList(&record->items, data)) {
        return NULL;
    }

    return record;
}

CsvRecordCollection* pushCsvRecordCollection(CsvRecordCollection* collection, CsvRecord* record)
{
    if (!collection || !record) {
        return NULL;
    }

    if (!pushList(&collection->records, record)) {
        return NULL;
    }

    return collection;
}

int sizeCsvRecordCollection(const CsvRecordCollection* collection)
{
    return sizeList(&collection->records);
}

int sizeCsvRecord(const CsvRecord* record)
{
    return sizeList(&record->items);
}

CsvRecord* getRecordCsvRecordCollection(const CsvRecordCollection* collection, int pos)
{
    return !collection || pos >= sizeCsvRecordCollection(collection) || pos < 0 ?
    NULL : (CsvRecord*) getDataList(&collection->records, pos);
}

void setRecordCsvRecordCollection(CsvRecordCollection* collection, int pos, CsvRecord* record)
{
    if (!collection || !record || pos < 0) {
        return;
    }
    setDataList(&collection->records, pos, record);
}

char* getFieldCsvRecord(const CsvRecord* record, int pos)
{
    return !record || pos >= sizeCsvRecord(record) || pos < 0 ?
    NULL : (char*) getDataList(&record->items, pos);
}

void* copyCsvRecord(const void* from)
{
    if (!from) {
        return NULL;
    }

    const CsvRecord* from_ = (const CsvRecord*) from;
    CsvRecord* to = (CsvRecord*) malloc(sizeof(CsvRecord));

    if (!to) {
        return NULL;
    }

    constructCsvRecord(to);

    if (!copyList(&to->items, &from_->items, dupString, free)) {
        destructCsvRecord(to);
        free(to);
        return NULL;
    }

    return to;
}

void* copyCsvRecordCollection(const void* from)
{
    if (!from) {
        return NULL;
    }

    CsvRecordCollection* from_ = (CsvRecordCollection*) from;
    CsvRecordCollection* to = (CsvRecordCollection*) malloc(sizeof(CsvRecordCollection));

    if (!to) {
        return NULL;
    }

    constructCsvRecordCollection(to);

    if (!copyList(&to->records, &from_->records, copyCsvRecord, destructCsvRecord)) {
        destructCsvRecordCollection(to);
        free(to);
        return NULL;
    }

    return to;
}

CsvRecordCollection* removeInvalidRecords(CsvRecordCollection* collection, int column)
{
    CsvRecordCollection* newCollection = (CsvRecordCollection*) malloc(sizeof(CsvRecordCollection));
    if (!newCollection) {
        return NULL;
    }

    constructCsvRecordCollection(newCollection);

    int columnCount = getColumnCountCsvRecordCollection(collection);
    int collectionSize = sizeCsvRecordCollection(collection);
    for (int i = 0; i < collectionSize; ++i) {
        CsvRecord* record = (CsvRecord*) copyCsvRecord(getRecordCsvRecordCollection(collection, i));
        if (isValidCsvRecord(record, columnCount) && getFieldCsvRecord(record, column - 1)) {
            pushCsvRecordCollection(newCollection, record);
        } else {
            destructCsvRecord(record);
            free(record);
        }
    }

    return newCollection;
}

CsvRecordCollection* sortCsvRecordCollection(CsvRecordCollection* collection,
                             int columnNo,
                             int (*compar)(const char*, const char*),
                             int (*isValidField)(const char*))
{

    if (!collection || columnNo <= 0 || !compar) {
        return NULL;
    }

    int size = sizeCsvRecordCollection(collection);

    for (int i = size - 1; i > 1; --i) {
        for (int j = 1; j < i; ++j) {
            CsvRecord* rec1 = getRecordCsvRecordCollection(collection, j);
            CsvRecord* rec2 = getRecordCsvRecordCollection(collection, j + 1);
            const char* field1 = getFieldCsvRecord(rec1, columnNo - 1);
            const char* field2 = getFieldCsvRecord(rec2, columnNo - 1);

            if (!isValidField(field1) || !isValidField(field2)) {
                return NULL;
            }

            if (compar(field1, field2) == 1) {
                CsvRecord* temp = rec1;
                setRecordCsvRecordCollection(collection, j, rec2);
                setRecordCsvRecordCollection(collection, j + 1, temp);
            }
        }
    }

    return collection;
}

int getColumnCountCsvRecordCollection(const CsvRecordCollection* collection)
{
    const CsvRecord* header = getRecordCsvRecordCollection(collection, 0);
    return sizeCsvRecord(header);
}
