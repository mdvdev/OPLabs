#ifndef DOMAINLOGIC_H
#define DOMAINLOGIC_H

#include "error.h"

struct AppData;

char* convertInput(const AppData* appData, const char* input);
Error validateInput(const AppData* appData, const char* input);
void setInputRadix(AppData* appData, int radix);
void setOutputRadix(AppData* appData, int radix);

#endif // DOMAINLOGIC_H
