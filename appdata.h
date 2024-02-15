#ifndef APPDATA_H
#define APPDATA_H

#include "error.h"

#define MAX_INPUT_LEN  32
#define MAX_OUTPUT_LEN MAX_INPUT_LEN

struct AppData {
    int inputRadix;
    int outputRadix;
    char input[MAX_INPUT_LEN + 1];
    char output[MAX_OUTPUT_LEN + 1];
    Error error;
};

#endif // APPDATA_H
