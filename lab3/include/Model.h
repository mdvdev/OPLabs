#ifndef MODEL_H
#define MODEL_H

struct AppData;

void initAppData(AppData* appData);
void setFileName(AppData* appData, const char* fileName);
void setRegion(AppData* appData, const char* region);
void setColumn(AppData* appData, const char* column);
void loadCsvFile(AppData* appData);
void calcMetrics(AppData* appData);

#endif // MODEL_H
