#include <stdlib.h>
#include <string.h>

#include "CsvParser.h"

static const char* processDelimeter(const char* begin, CsvRecord* record)
{
    if (!begin || !record) {
        return NULL;
    }

    if (*begin != ',') {
        return begin;
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
    if (!begin || !record) {
        return NULL;
    }

    if (*begin == ',') {
        return begin;
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

    CsvRecord* record = constructRecord();
    if (!record) {
        return NULL;
    }

    const char* begin = str;

    while (*begin) {
        if (*begin == ',') {
            begin = processDelimeter(begin, record);
            if (!begin) {
                destructRecord(record);
                record = NULL;
                break;
            }
        } else {
            begin = processToken(begin, record);
            if (!begin) {
                destructRecord(record);
                record = NULL;
                break;
            }
        }
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
        return NULL;
    }

    return collection;
}

CsvRecordCollection* parseCsv(FILE* fp)
{
    if (!fp) {
        return NULL;
    }

    CsvRecordCollection* collection = constructCollection();
    if (!collection) {
        return NULL;
    }

    char* buf = NULL;
    ssize_t rc;
    size_t num;

    while ((rc = getline(&buf, &num, fp)) != -1) {
        buf[rc-1] = '\0';
        if (!processString(buf, collection)) {
            destructCollection(collection);
            collection = NULL;
            break;
        }
    }

    free(buf);

    if (ferror(fp)) {
        destructCollection(collection);
        collection = NULL;
    }

    return collection;
}

CsvRecordCollection* constructCollection()
{
    CsvRecordCollection* collection = (CsvRecordCollection*) malloc(sizeof(CsvRecordCollection));
    if (!collection) {
        return NULL;
    }

    collection->records = createList();

    return collection;
}

void destructCollection(void* collection)
{
    if (!collection) {
        return;
    }
    CsvRecordCollection* collection_ = (CsvRecordCollection*) collection;
    freeList(&collection_->records, destructRecord);
    free(collection);
}

CsvRecord* constructRecord()
{
    CsvRecord* record = (CsvRecord*) malloc(sizeof(CsvRecord));
    if (!record) {
        return NULL;
    }

    record->items = createList();

    return record;
}

void destructRecord(void* record)
{
    if (!record) {
        return;
    }
    CsvRecord* record_ = (CsvRecord*) record;
    freeList(&record_->items, free);
    free(record);
}

CsvRecord* pushCsvRecord(CsvRecord* record, void* data)
{
    if (!record) {
        return NULL;
    }

    ListItem* item = constructListItem(data);
    if (!item || !pushList(&record->items, item)) {
        free(item);
        return NULL;
    }

    return record;
}

CsvRecordCollection* pushCsvRecordCollection(CsvRecordCollection* collection, CsvRecord* record)
{
    if (!collection || !record) {
        return NULL;
    }

    ListItem* record_ = constructListItem(record);
    if (!record || !pushList(&collection->records, record_)) {
        free(record_);
        return NULL;
    }

    return collection;
}

size_t sizeCsvRecordCollection(const CsvRecordCollection* collection)
{
    return sizeList(&collection->records);
}

size_t sizeCsvRecord(const CsvRecord* record)
{
    return sizeList(&record->items);
}

const CsvRecord* getRecordCsvRecordCollection(const CsvRecordCollection* collection, size_t pos)
{
    return (const CsvRecord*) getDataList(&collection->records, pos);
}

const char* getFieldCsvRecord(const CsvRecord* record, size_t pos)
{
    return (const char*) getDataList(&record->items, pos);
}
