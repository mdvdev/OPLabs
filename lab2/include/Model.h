#ifndef MODEL_H
#define MODEL_H

struct AppData;

void initAppData(AppData* appData);
void saveFileName(AppData* appData, const char* fileName);
void loadCsvFile(AppData* appData);

#endif // MODEL_H
