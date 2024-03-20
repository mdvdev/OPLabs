#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Operation.h"

struct AppData;

struct Params {
    const char* fileName;
};

void doOperation(Operation operation, AppData* appData, const Params* params);

#endif // CONTROLLER_H
