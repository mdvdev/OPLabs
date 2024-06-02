#include <stdlib.h>
#include <string.h>
#include <cassert>

#include "CsvParser.h"

static const char* processDelimeter(const char* begin, CsvRecord* record)
{
    assert(*begin == ',');

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
    assert(*begin != ',');

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
    CsvRecord* record = stringToCsvRecord(str);
    if (!record) {
        return NULL;
    }

    int columnCount = getColumnCountCsvRecordCollection(collection);

    if (columnCount == 0 || sizeCsvRecord(record) == columnCount) {
        if (!pushCsvRecordCollection(collection, record)) {
            destructCsvRecord(record);
            free(record);
            return NULL;
        }
    }

    return collection;
}

static void* dupString(const void* str)
{
    const char* str_ = (const char*) str;
    return strdup(str_);
}

int isValidCsvRecord(const CsvRecord* record, int column)
{
    return column <= 0 ? 0 : sizeCsvRecord(record) == column;
}

CsvRecordCollection* parseCsv(FILE* fp)
{
    CsvRecordCollection* collection = (CsvRecordCollection*) malloc(sizeof(CsvRecordCollection));
    if (!collection) {
        return NULL;
    }

    constructCsvRecordCollection(collection);

    char* buf = NULL;
    ssize_t rc;
    size_t num;

    while ((rc = getline(&buf, &num, fp)) != -1) {
        buf[rc - 1] = '\0';
        if (!processString(buf, collection)) {
            destructCsvRecordCollection(collection, false);
            free(collection);
            collection = NULL;
            break;
        }
    }

    free(buf);

    if (ferror(fp)) {
        destructCsvRecordCollection(collection, false);
        free(collection);
        collection = NULL;
    }

    return collection;
}

void constructCsvRecordCollection(CsvRecordCollection* collection)
{
    constructList(&collection->records);
}

void destructCsvRecordCollection(void* collection, int weakOwnership)
{
    CsvRecordCollection* collection_ = (CsvRecordCollection*) collection;
    destructList(&collection_->records, weakOwnership ? NULL : destructCsvRecord);
}

void constructCsvRecord(CsvRecord* record)
{
    constructList(&record->items);
}

void destructCsvRecord(void* record)
{
    CsvRecord* record_ = (CsvRecord*) record;
    destructList(&record_->items, free);
}

CsvRecord* pushCsvRecord(CsvRecord* record, const char* data)
{
    if (!pushList(&record->items, (void*) data)) {
        return NULL;
    }

    return record;
}

CsvRecordCollection* pushCsvRecordCollection(CsvRecordCollection* collection, CsvRecord* record)
{
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
    return pos >= sizeCsvRecordCollection(collection) || pos < 0 ?
    NULL : (CsvRecord*) getDataList(&collection->records, pos);
}

void setRecordCsvRecordCollection(CsvRecordCollection* collection, int pos, CsvRecord* record)
{
    assert(pos >= 0);
    setDataList(&collection->records, pos, record);
}

char* getFieldCsvRecord(const CsvRecord* record, int pos)
{
    return pos >= sizeCsvRecord(record) || pos < 0 ?
    NULL : (char*) getDataList(&record->items, pos);
}

void* copyCsvRecord(const void* from)
{
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
    CsvRecordCollection* from_ = (CsvRecordCollection*) from;
    CsvRecordCollection* to = (CsvRecordCollection*) malloc(sizeof(CsvRecordCollection));

    if (!to) {
        return NULL;
    }

    constructCsvRecordCollection(to);

    if (!copyList(&to->records, &from_->records, copyCsvRecord, destructCsvRecord)) {
        destructCsvRecordCollection(to, false);
        free(to);
        return NULL;
    }

    return to;
}

CsvRecordCollection* sortCsvRecordCollection(CsvRecordCollection* collection,
                             int columnNo,
                             int (*compar)(const char*, const char*),
                             int (*isValidField)(const char*))
{
    assert(columnNo > 0);

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
