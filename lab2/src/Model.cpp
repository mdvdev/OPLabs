#include <stdio.h>
#include <string.h>
#include <math.h>

#include "Model.h"
#include "AppData.h"

#include "CsvParser.h"

#define DEFAULT_CALC_INVALID_VALUE 0
#define EPSILON 0.0001

static int strToFloat(const char* str, float* n)
{
    return sscanf(str, "%f", n) == 1;
}

static int strToInt(const char* str, int* n)
{
    return sscanf(str, "%d", n) == 1;
}

static Error setInitValAccumulator(const CsvRecordCollection* collection,
                                 const char* region,
                                 int columnNo, float* accum, int* startPos)
{
    const CsvRecord* record = NULL;
    int collectionSize = sizeCsvRecordCollection(collection);
    int columnCount = getColumnCountCsvRecordCollection(collection);

    for (int i = 1; i < collectionSize; ++i) {
        record = getRecordCsvRecordCollection(collection, i);
        const char* recordRegion = getFieldCsvRecord(record, 1);
        if (!recordRegion) {
            continue;
        }
        if (isValidCsvRecord(record, columnCount) &&
            (strcmp(region, "") == 0 ||
            strcmp(region, recordRegion) == 0))
        {
            const char* field = getFieldCsvRecord(record, columnNo - 1);
            if (!field) {
                continue;
            }
            if (!strToFloat(field, accum)) {
                return INVALID_FIELD_ERROR;
            }
            *startPos = i;
            return NO_ERROR;
        }
    }

    return REGION_NOT_EXIST;
}

static Error processField(const CsvRecordCollection* collection,
                          const char* region,
                          int columnNo,
                          float* accum, int pos,
                          void (*callback)(float*, float))
{
    const CsvRecord* record = getRecordCsvRecordCollection(collection, pos);
    int columnCount = getColumnCountCsvRecordCollection(collection);

    if (!isValidCsvRecord(record, columnCount)) {
        return SKIP_RECORD_ERROR;
    }

    const char* field = getFieldCsvRecord(record, columnNo - 1);
    if (!field) {
        return SKIP_RECORD_ERROR;
    }

    float num;
    if (!strToFloat(field, &num)) {
        return INVALID_FIELD_ERROR;
    }

    if (strcmp(region, "") == 0 ||
        strcmp(region, getFieldCsvRecord(record, 1)) == 0)
    {
        callback(accum, num);
    }

    return NO_ERROR;
}

static Error mapCsvRecordCollection(const CsvRecordCollection* collection,
                                  const char* column,
                                  const char* region,
                                  void (*callback)(float*, float),
                                  float* n)
{
    if (!collection || !column || !callback || strcmp(column, "") == 0 || !n) {
        return INVALID_COLUMN_ERROR;
    }

    int columnCount = getColumnCountCsvRecordCollection(collection);
    int columnNo;

    if (!strToInt(column, &columnNo) || columnNo > columnCount || columnNo <= 0) {
        return INVALID_COLUMN_ERROR;
    }

    float accum;
    int startPos;

    Error error;
    if ((error = setInitValAccumulator(collection, region, columnNo, &accum, &startPos)) != NO_ERROR) {
        return error;
    }

    int collectionSize = sizeCsvRecordCollection(collection);

    for (int i = startPos; i < collectionSize; ++i) {
        if ((error = processField(collection, region, columnNo, &accum, i, callback)) != NO_ERROR) {
            if (error == SKIP_RECORD_ERROR) {
                continue;
            }
            return error;
        }
    }

    *n = accum;

    return NO_ERROR;
}

static char* floatToString(float n)
{
    int len = snprintf(NULL, 0, "%.2f", n);
    char* result = (char*) malloc(len + 1);

    if (!result) {
        return NULL;
    }

    snprintf(result, len + 1, "%.2f", n);

    return result;
}

static void minCallback(float* accum, float n)
{
    if (!accum) {
        return;
    }

    if (n < *accum) {
        *accum = n;
    }
}

static void maxCallback(float* accum, float n)
{
    if (!accum) {
        return;
    }

    if (n > *accum) {
        *accum = n;
    }
}

static int floatComparator(const char* a, const char* b)
{
    if (!a || !b) {
        return -1;
    }

    float tempA, tempB;
    if (!strToFloat(a, &tempA) || !strToFloat(b, &tempB)) {
        return -1;
    }

    if (tempA > tempB) {
        return 1;
    } else if (fabs(tempB - tempA) < EPSILON) {
        return 0;
    } else {
        return -1;
    }
}

static int isValidFloatField(const char* a)
{
    if (!a) {
        return 0;
    }

    float tempA;

    return strToFloat(a, &tempA);
}

static void calcMinimum(AppData* appData)
{
    float min;
    if ((appData->error = mapCsvRecordCollection(appData->records,
                                        appData->column.begin,
                                        appData->region.begin,
                                        minCallback, &min)) != NO_ERROR)
    {
        if (appData->error == REGION_NOT_EXIST) {
            assignString(&appData->min, "");
        }
        return;
    }

    char* res = floatToString(min);
    assignString(&appData->min, res);
    free(res);
}

static void calcMaximum(AppData* appData)
{
    float max;
    if ((appData->error = mapCsvRecordCollection(appData->records,
                                        appData->column.begin,
                                        appData->region.begin,
                                        maxCallback, &max)) != NO_ERROR)
    if (appData->error != NO_ERROR) {
        if (appData->error == REGION_NOT_EXIST) {
            assignString(&appData->max, "");
        }
        return;
    }

    char* res = floatToString(max);
    assignString(&appData->max, res);
    free(res);
}

static void calcMedian(AppData* appData)
{
    if (!appData) {
        return;
    }

    const CsvRecord* header = getRecordCsvRecordCollection(appData->records, 0);
    int columnCount = sizeCsvRecord(header);
    int column;

    if (!strToInt(appData->column.begin, &column) || column >= columnCount || column <= 0) {
        appData->error = INVALID_COLUMN_ERROR;
        return;
    }

    CsvRecordCollection* collectionCopy = (CsvRecordCollection*) copyCsvRecordCollection(appData->records);
    if (!sortCsvRecordCollection(collectionCopy, column, floatComparator, isValidFloatField)) {
        appData->error = INVALID_FIELD_ERROR;
        destructCsvRecordCollection(collectionCopy);
        free(collectionCopy);
        return;
    }

    int middleIndex = (sizeCsvRecordCollection(collectionCopy) - 1) / 2;
    CsvRecord* middleRecord = getRecordCsvRecordCollection(collectionCopy, middleIndex);
    const char* field = getFieldCsvRecord(middleRecord, column);
    assignString(&appData->median, field);

    destructCsvRecordCollection(collectionCopy);
    free(collectionCopy);
}

void initAppData(AppData* appData)
{
    if (!appData) {
        return;
    }

    appData->error = NO_ERROR;
    appData->records = NULL;

    constructString(&appData->fileName, "");
    constructString(&appData->region, "");
    constructString(&appData->column, "");
    constructString(&appData->min, "");
    constructString(&appData->max, "");
    constructString(&appData->median, "");
}

void setFileName(AppData* appData, const char* fileName)
{
    if (!appData || !fileName) {
        return;
    }

    assignString(&appData->fileName, fileName);
}

void loadCsvFile(AppData* appData)
{
    if (!appData || !appData->fileName.begin || strcmp(appData->fileName.begin, "") == 0) {
        return;
    }

    appData->error = NO_ERROR;

    FILE* fp = fopen(appData->fileName.begin, "r");
    if (!fp) {
        appData->error = OPEN_FILE_ERROR;
        return;
    }

    if (appData->records) {
        destructCsvRecordCollection(appData->records);
        free(appData->records);
    }

    appData->records = parseCsv(fp);
    if (!appData->records) {
        appData->error = PARSE_CSV_ERROR;
    }

    fclose(fp);
}

void setRegion(AppData* appData, const char* region)
{
    if (!appData || !region) {
        return;
    }

    assignString(&appData->region, region);
}

void setColumn(AppData* appData, const char* column)
{
    if (!appData || !column) {
        return;
    }

    assignString(&appData->column, column);
    appData->error = NO_ERROR;

    if (*column == '\0') {
        return;
    }

    char* end = NULL;
    long int tempColumn = strtol(column, &end, 10);
    const CsvRecord* header = getRecordCsvRecordCollection(appData->records, 0);
    int columnCount = sizeCsvRecord(header);

    if (*end != '\0' || tempColumn <= 0 || tempColumn > columnCount) {
        appData->error = INVALID_COLUMN_ERROR;
    }
}

void calcMetrics(AppData* appData)
{
    if (!appData || !appData->records ||
        strcmp(appData->column.begin, "") == 0)
    {
        return;
    }

    calcMinimum(appData);
    if (appData->error != NO_ERROR) {
        return;
    }

    calcMaximum(appData);
    if (appData->error != NO_ERROR) {
        return;
    }

    //calcMedian(appData);
}
