#include <stdio.h>
#include <string.h>
#include <cassert>
#include <cmath>

#include "Model.h"
#include "AppData.h"

#include "CsvParser.h"
#include "Point.h"
#include "List.h"

struct Params {
    const char* axis;
    float factor;
    float from;
    float to;
    AppData* appData;
};

static int strToFloat(const char* str, float* n)
{
    return sscanf(str, "%f", n) == 1;
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

static int getErrorCount(const CsvRecordCollection* records)
{
    int errorCount = 0;
    int collectionSize = sizeCsvRecordCollection(records);
    int columnCount = getColumnCountCsvRecordCollection(records);
    for (int i = 0; i < collectionSize; ++i) {
        const CsvRecord* record = getRecordCsvRecordCollection(records, i);
        if (!isValidCsvRecord(record, columnCount)) {
            errorCount++;
        }
    }

    return errorCount;
}

static void setErrors(AppData* appData, const CsvRecordCollection* records)
{
    int errorCount = getErrorCount(records);
    int totalCount = sizeCsvRecordCollection(records);
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

static List* convertCsvToPointMatrix(AppData* appData, const CsvRecordCollection* records)
{
    List* pointMatrix = (List*) malloc(sizeof(List));
    if (!pointMatrix) {
        return NULL;
    }

    constructList(pointMatrix);

    int sizeCollection = sizeCsvRecordCollection(records);
    for (int i = 0; i < sizeCollection; ++i) {
        List* pointRecord = (List*) malloc(sizeof(List));
        if (!pointRecord) {
            destructCsvRecordCollection(pointMatrix, false);
            free(pointMatrix);
            return NULL;
        }

        constructList(pointRecord);

        CsvRecord* record = getRecordCsvRecordCollection(records, i);
        int recordSize = sizeCsvRecord(record);
        for (int j = 0; j < recordSize; ++j) {
            Point* point = (Point*) malloc(sizeof(Point));
            if (!point) {
                destructCsvRecord(pointRecord);
                free(pointRecord);
                destructCsvRecordCollection(pointMatrix, false);
                free(pointMatrix);
                return NULL;
            }
            point->x = j;
            point->y = i;
            if (!strToFloat(getFieldCsvRecord(record, j), &point->z)) {
                appData->error = INVALID_RECORD_ERROR;
                free(point);
                destructCsvRecord(pointRecord);
                free(pointRecord);
                destructCsvRecordCollection(pointMatrix, false);
                free(pointMatrix);
                return NULL;
            }
            pushList(pointRecord, point);
        }

        pushList(pointMatrix, pointRecord);
    }

    return pointMatrix;
}

static float findMinValue(AppData* appData, const char* axis)
{
    assert(sizeList(appData->pointMatrix) != 0);
    assert(strcmp(axis, "X") == 0 || strcmp(axis, "Y") == 0 || strcmp(axis, "Z") == 0);

    List* record = (List*) getDataList(appData->pointMatrix, 0);
    Point* point = (Point*) getDataList(record, 0);
    float min = strcmp(axis, "X") == 0 ? point->x : strcmp(axis, "Y") == 0 ? point->y : point->z;
    int size = sizeList(appData->pointMatrix);

    for (int i = 0; i < size; ++i) {
        record = (List*) getDataList(appData->pointMatrix, i);
        int recordSize = sizeList(record);
        for (int j = 0; j < recordSize; ++j) {
            if (i == 0 && j == 0) {
                continue;
            }
            point = (Point*) getDataList(record, j);
            if (strcmp(axis, "X") == 0 && point->x < min) {
                min = point->x;
            } else if (strcmp(axis, "Y") == 0 && point->y < min) {
                min = point->y;
            } else if (point->z < min) {
                min = point->z;
            }
        }
    }

    return min;
}

static float findMaxValue(AppData* appData, const char* axis)
{
    assert(sizeList(appData->pointMatrix) != 0);
    assert(strcmp(axis, "X") == 0 || strcmp(axis, "Y") == 0 || strcmp(axis, "Z") == 0);

    List* record = (List*) getDataList(appData->pointMatrix, 0);
    Point* point = (Point*) getDataList(record, 0);
    float max = strcmp(axis, "X") == 0 ? point->x : strcmp(axis, "Y") == 0 ? point->y : point->z;
    int size = sizeList(appData->pointMatrix);

    for (int i = 0; i < size; ++i) {
        record = (List*) getDataList(appData->pointMatrix, i);
        int recordSize = sizeList(record);
        for (int j = 0; j < recordSize; ++j) {
            if (i == 0 && j == 0) {
                continue;
            }
            point = (Point*) getDataList(record, j);
            if (strcmp(axis, "X") == 0 && point->x > max) {
                max = point->x;
            } else if (strcmp(axis, "Y") == 0 && point->y > max) {
                max = point->y;
            } else if (point->z > max) {
                max = point->z;
            }
        }
    }

    return max;
}

static float normalizeValue(AppData* appData, const char* axis, float value,  float from, float to)
{
    assert(from < to);
    float min = findMinValue(appData, axis);
    float max = findMaxValue(appData, axis);
    return min == max ? value : from + (value - min) / (max - min) * (to - from);
}

static void scaleShapeCallback(Point* point, void* data)
{
    float scaleFactor = *(float*) data;
    point->x *= scaleFactor;
    point->y *= scaleFactor;
    point->z *= scaleFactor;
}

static void moveShapeCallback(Point* point, void* data)
{
    Params params = *(Params*) data;
    const char* axis = params.axis;
    float moveFactor = params.factor;

    if (strcmp(axis, "X") == 0) {
        point->x += moveFactor;
    } else if (strcmp(axis, "Y") == 0) {
        point->y += moveFactor;
    } else {
        point->z += moveFactor;
    }
}

static void rotateShapeCallback(Point* point, void* data)
{
    Params params = *(Params*) data;
    const char* axis = params.axis;
    float rotateFactor = params.factor;
    float angle = rotateFactor / 10;

    if (strcmp(axis, "X") == 0) {
        float savedY = point->y;
        point->y = savedY * cos(angle) - point->z * sin(angle);
        point->z = savedY * sin(angle) + point->z * cos(angle);
    } else if (strcmp(axis, "Y") == 0) {
        float savedX = point->x;
        point->x = savedX * cos(angle) + point->z * sin(angle);
        point->z = -savedX * sin(angle) + point->z * cos(angle);
    } else {
        float savedX = point->x;
        point->x = savedX * cos(angle) - point->y * sin(angle);
        point->y = savedX * sin(angle) + point->y * cos(angle);
    }
}

static void normalizeShapeCallback(Point* point, void* data)
{
    Params params = *(Params*) data;
    AppData* appData = params.appData;
    float from = params.from;
    float to = params.to;

    point->x = normalizeValue(appData, "X", point->x, from, to);
    point->y = normalizeValue(appData, "Y", point->y, from, to);
    point->z = normalizeValue(appData, "Z", point->z, from, to);
}

static void traversePointMatrix(AppData* appData, void (*cb)(Point*, void*), void* data)
{
    int size = sizeList(appData->pointMatrix);
    for (int i = 0; i < size; ++i) {
        List* record = (List*) getDataList(appData->pointMatrix, i);
        int recordSize = sizeList(record);
        for (int j = 0; j < recordSize; ++j) {
            Point* point = (Point*) getDataList(record, j);
            cb(point, data);
        }
    }
}

void initAppData(AppData* appData)
{
    appData->error = NO_ERROR;
    appData->pointMatrix = NULL;

    constructString(&appData->normalizeFrom, "");
    constructString(&appData->normalizeTo, "");
    constructString(&appData->errorCount, "");
    constructString(&appData->totalCount, "");
    constructString(&appData->validCount, "");
    constructString(&appData->fileName, "");
}

void setFileName(AppData* appData, const char* fileName)
{
    assignString(&appData->fileName, fileName);
}

void loadCsvFile(AppData* appData)
{
    if (strcmp(appData->fileName.begin, "") == 0) {
        return;
    }

    appData->error = NO_ERROR;

    FILE* fp = fopen(appData->fileName.begin, "r");
    if (!fp) {
        appData->error = OPEN_FILE_ERROR;
        return;
    }

    if (appData->pointMatrix) {
        destructCsvRecordCollection(appData->pointMatrix, false);
        free(appData->pointMatrix);
    }

    CsvRecordCollection* records = parseCsv(fp);
    if (!records) {
        appData->error = PARSE_CSV_ERROR;
    } else {
        setErrors(appData, records);
        appData->pointMatrix = convertCsvToPointMatrix(appData, records);
        free(records);
    }

    fclose(fp);
}

void scaleShape(AppData* appData, float scaleFactor)
{
    assert(scaleFactor > 0);
    if (!appData->pointMatrix) {
        return;
    }
    traversePointMatrix(appData, scaleShapeCallback, &scaleFactor);
}

void moveShape(AppData* appData, const char* axis, float moveFactor)
{
    assert(strcmp(axis, "X") == 0 || strcmp(axis, "Y") == 0 || strcmp(axis, "Z") == 0);
    assert(moveFactor != 0);
    if (!appData->pointMatrix) {
        return;
    }
    Params params = {.axis = axis, .factor = moveFactor};
    traversePointMatrix(appData, moveShapeCallback, &params);
}

void rotateShape(AppData* appData, const char* axis, float rotateFactor)
{
    assert(strcmp(axis, "X") == 0 || strcmp(axis, "Y") == 0 || strcmp(axis, "Z") == 0);
    assert(rotateFactor != 0);
    if (!appData->pointMatrix) {
        return;
    }
    Params params = {.axis = axis, .factor = rotateFactor};
    traversePointMatrix(appData, rotateShapeCallback, &params);
}

void normalizeShape(AppData* appData, const char* fromNormalization, const char* toNormalization)
{
    if (!appData->pointMatrix) {
        return;
    }

    assignString(&appData->normalizeFrom, fromNormalization);
    assignString(&appData->normalizeTo, toNormalization);

    // No normalization needed
    if (strcmp(fromNormalization, "") == 0 && strcmp(toNormalization, "") == 0) {
        return;
    }

    float from;
    float to;
    if (!strToFloat(fromNormalization, &from) || !strToFloat(toNormalization, &to) || from >= to) {
        appData->error = INVALID_RANGE_ERROR;
        return;
    }

    Params params = {.from = from, .to = to, .appData = appData};
    traversePointMatrix(appData, normalizeShapeCallback, &params);
}
