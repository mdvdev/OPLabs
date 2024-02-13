#ifndef CONTROLLER_H
#define CONTROLLER_H

class MainWindow;
struct AppData;

struct AppParams {
    MainWindow* mainWindow;
    const char* input;
    int radix;
};

void processConvertClick(AppData* appData, const AppParams* params);
void processOutputRadixSelect(AppData* appData, const AppParams* params);
void processInputRadixSelect(AppData* appData, const AppParams* params);
void processChangedInputLine(AppData* appData, const AppParams* params);

#endif // CONTROLLER_H
