#ifndef DOMAINLOGIC_H
#define DOMAINLOGIC_H

struct AppData;

void setInputRadix(AppData* appData, int radix);
void setOutputRadix(AppData* appData, int radix);
void setInput(AppData* appData, const char* input);
void setOutput(AppData* appData, const char* output);
void convertInput(AppData* appData);

#endif // DOMAINLOGIC_H
