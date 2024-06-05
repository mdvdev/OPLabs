#include "controller.h"
#include "appdata.h"
#include "domainlogic.h"

void processInput(Operation operation, AppData* appData, const AppParams* params)
{
    switch (operation) {
    case Convert:
        convertInput(appData);
        break;
    case Swap:
        swapInputToOutput(appData);
        break;
    case CopyOutput:
        break;
    case SelectOutputRadix:
        setOutputRadix(appData, params->radix);
        break;
    case SelectInputRadix:
        setInputRadix(appData, params->radix);
        break;
    case EditInputLine:
        setInput(appData, params->input);
        break;
    }
}
