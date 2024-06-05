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

static char* intToString(int n)
{
    int len = snprintf(NULL, 0, "%d", n);
    char* result = (char*) malloc(len + 1);

    if (!result) {
        return NULL;
    }

    snprintf(result, len + 1, "%d", n);

    return result;
}

static Error minMaxSetInitVal(AppData* appData, int columnNo, float* min, float* max, int* startPos)
{
    const CsvRecord* record = NULL;
    int collectionSize = sizeCsvRecordCollection(appData->records);
    int columnCount = getColumnCountCsvRecordCollection(appData->records);

    for (int i = 1; i < collectionSize; ++i) {
        record = getRecordCsvRecordCollection(appData->records, i);
        const char* recordRegion = getFieldCsvRecord(record, 1);
        if (!recordRegion) {
            continue;
        }
        if (isValidCsvRecord(record, columnCount) &&
            (strcmp(appData->region.begin, "") == 0 ||
            strcmp(appData->region.begin, recordRegion) == 0))
        {
            const char* field = getFieldCsvRecord(record, columnNo - 1);
            if (!field) {
                continue;
            }
            if (!strToFloat(field, min)) {
                return INVALID_FIELD_ERROR;
            }
            *max = *min;
            *startPos = i;
            return NO_ERROR;
        }
    }

    return REGION_NOT_EXIST;
}

static Error processField(AppData* appData,
                          int columnNo,
                          float* min, float* max,
                          int pos)
{
    const CsvRecord* record = getRecordCsvRecordCollection(appData->records, pos);
    int columnCount = getColumnCountCsvRecordCollection(appData->records);

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

    if (strcmp(appData->region.begin, "") == 0 ||
        strcmp(appData->region.begin, getFieldCsvRecord(record, 1)) == 0)
    {
        if (num > *max) {
            *max = num;
        } else if (num < *min) {
            *min = num;
        }
    }

    return NO_ERROR;
}

static void toStringMinMax(AppData* appData, float min, float max)
{
    char* minStr = floatToString(min);
    char* maxStr = floatToString(max);

    assignString(&appData->min, minStr);
    assignString(&appData->max, maxStr);

    free(minStr);
    free(maxStr);
}

static Error calcMinMax(AppData* appData)
{
    if (!appData || strcmp(appData->column.begin, "") == 0) {
        return INVALID_COLUMN_ERROR;
    }

    int columnCount = getColumnCountCsvRecordCollection(appData->records);
    int columnNo;

    if (!strToInt(appData->column.begin, &columnNo) || columnNo > columnCount || columnNo <= 0) {
        return INVALID_COLUMN_ERROR;
    }

    int startPos;
    float min, max;
    Error error;

    if ((error = minMaxSetInitVal(appData, columnNo, &min, &max, &startPos)) != NO_ERROR) {
        return error;
    }

    int collectionSize = sizeCsvRecordCollection(appData->records);

    for (int i = startPos; i < collectionSize; ++i) {
        if ((error = processField(appData, columnNo, &min, &max, i)) != NO_ERROR) {
            if (error == SKIP_RECORD_ERROR) {
                continue;
            }
            return error;
        }
    }

    toStringMinMax(appData, min, max);

    return NO_ERROR;
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

static Error calcMedian(AppData* appData)
{
    if (!appData || strcmp(appData->column.begin, "") == 0) {
        return INVALID_COLUMN_ERROR;
    }

    const CsvRecord* header = getRecordCsvRecordCollection(appData->records, 0);
    int columnCount = sizeCsvRecord(header);
    int columnNo;

    if (!strToInt(appData->column.begin, &columnNo) || columnNo > columnCount || columnNo <= 0) {
        return INVALID_COLUMN_ERROR;
    }

    CsvRecordCollection* clearCollection = removeInvalidRecords(appData->records, columnNo);

    if (!sortCsvRecordCollection(clearCollection, columnNo, floatComparator, isValidFloatField)) {
        destructCsvRecordCollection(clearCollection);
        free(clearCollection);
        return INVALID_FIELD_ERROR;
    }

    int middleIndex = (sizeCsvRecordCollection(clearCollection) - 2) / 2;
    CsvRecord* middleRecord = getRecordCsvRecordCollection(clearCollection, middleIndex);
    const char* field = getFieldCsvRecord(middleRecord, columnNo - 1);

    assignString(&appData->median, field);

    destructCsvRecordCollection(clearCollection);
    free(clearCollection);

    return NO_ERROR;
}

static int getErrorCount(AppData* appData)
{
    if (!appData) {
        return 0;
    }

    int errorCount = 0;
    int collectionSize = sizeCsvRecordCollection(appData->records);
    int columnCount = getColumnCountCsvRecordCollection(appData->records);
    for (int i = 0; i < collectionSize; ++i) {
        const CsvRecord* record = getRecordCsvRecordCollection(appData->records, i);
        if (!isValidCsvRecord(record, columnCount)) {
            errorCount++;
        }
    }

    return errorCount;
}

static void setErrors(AppData* appData)
{
    int errorCount = getErrorCount(appData);
    int totalCount = sizeCsvRecordCollection(appData->records);
    int validCount = totalCount - errorCount;

    char* errorCountStr = intToString(errorCount);
    char* totalCountStr = intToString(totalCount);
    char* validCountStr = intToString(validCount);

    assignString(&appData->errorCount, errorCountStr);
    assignString(&appData->totalCount, totalCountStr);
    assignString(&appData->validCount, validCountStr);

    free(errorCountStr);
    free(totalCountStr);
    free(validCountStr);
}

static void resetMetrics(AppData* appData)
{
    if (!appData) {
        return;
    }
    assignString(&appData->min, "");
    assignString(&appData->max, "");
    assignString(&appData->median, "");
}

void initAppData(AppData* appData)
{
    if (!appData) {
        return;
    }

    appData->error = NO_ERROR;
    appData->records = NULL;

    constructString(&appData->errorCount, "");
    constructString(&appData->totalCount, "");
    constructString(&appData->validCount, "");
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
    } else {
        setErrors(appData);
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

    if ((appData->error = calcMinMax(appData)) != NO_ERROR) {
        resetMetrics(appData);
        return;
    }

    if ((appData->error = calcMedian(appData)) != NO_ERROR) {
        resetMetrics(appData);
    }
}
