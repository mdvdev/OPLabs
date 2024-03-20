#include <stdio.h>
#include <string.h>

#include "Model.h"
#include "AppData.h"

#include "CsvParser.h"

void initAppData(AppData* appData)
{
    appData->error = NO_ERROR;
    appData->records = NULL;
    initString(&appData->fileName, "");
    initString(&appData->region, "");
    appData->column = 0;
    appData->min = appData->max = appData->median = 0;
}

void saveFileName(AppData* appData, const char* fileName)
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

    if (appData->records) {
        destructCollection(appData->records);
    }

    appData->records = parseCsv(fp);
    if (!appData->records) {
        appData->error = PARSE_CSV_ERROR;
    }

    fclose(fp);
}
