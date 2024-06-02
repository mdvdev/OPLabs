#ifndef MODEL_H
#define MODEL_H

struct AppData;

void initAppData(AppData* appData);
void setFileName(AppData* appData, const char* fileName);
void loadCsvFile(AppData* appData);
void scaleShape(AppData* appData, float scaleFactor);
void moveShape(AppData* appData, const char* axis, float moveFactor);
void rotateShape(AppData* appData, const char* axis, float rotateFactor);
void normalizeShape(AppData* appData, const char* from, const char* to);

#endif // MODEL_H
