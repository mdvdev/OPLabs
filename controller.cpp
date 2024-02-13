#include <stdlib.h>

#include "controller.h"
#include "domainlogic.h"
#include "error.h"
#include "mainwindow.h"

void processConvertClick(AppData* appData, const AppParams* params)
{
    char* convertedInput = convertInput(appData, params->input);
    params->mainWindow->updateOutputLine(convertedInput);
    free(convertedInput);
}

void processOutputRadixSelect(AppData* appData, const AppParams* params)
{
    setOutputRadix(appData, params->radix);
}

void processInputRadixSelect(AppData* appData, const AppParams* params)
{
    setInputRadix(appData, params->radix);
}

void processChangedInputLine(AppData* appData, const AppParams* params)
{
    Error validateRes = validateInput(appData, params->input);
    switch (validateRes) {
    case OutOfRange:
        params->mainWindow->updateErrorLabel("Entered number out of range");
        params->mainWindow->disableConvertButton();
        break;
    case InvalidChar:
        params->mainWindow->updateErrorLabel("Entered number contains invalid character");
        params->mainWindow->disableConvertButton();
        break;
    case NoError:
        params->mainWindow->updateErrorLabel("No errors");
        params->mainWindow->enableConvertButton();
        break;
    }
}
