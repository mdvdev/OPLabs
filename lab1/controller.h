#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "operation.h"

struct AppData;

struct AppParams {
    const char* input;
    int radix;
};

void processInput(Operation operation, AppData* appData, const AppParams* params);

#endif // CONTROLLER_H
