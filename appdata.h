#ifndef APPDATA_H
#define APPDATA_H

#include "error.h"
#include "charvector.h"

#define MAX_OUTPUT_LEN 32

struct AppData {
    int inputRadix;
    int outputRadix;
    Error error;
    CharVector input;
    char output[MAX_OUTPUT_LEN + 1];
};

#endif // APPDATA_H
