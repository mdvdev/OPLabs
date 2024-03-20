#include "Presenter.h"
#include "Model.h"

void doOperation(Operation operation, AppData* appData, const Params* params)
{
    switch (operation) {
    case INIT:
        initAppData(appData);
        break;
    case OPEN_FILE:
        saveFileName(appData, params->fileName);
        break;
    case LOAD_DATA:
        loadCsvFile(appData);
        break;
    case INPUT_REGION:
        break;
    case INPUT_COLUMN:
        break;
    case CALC_METRICS:
        break;
    }
}
