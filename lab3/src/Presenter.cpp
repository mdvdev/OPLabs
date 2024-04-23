#include "Presenter.h"
#include "Model.h"

void doOperation(Operation operation, AppData* appData, const Params* params)
{
    switch (operation) {
    case INIT:
        initAppData(appData);
        break;
    case OPEN_FILE:
        setFileName(appData, params->fileName);
        break;
    case LOAD_DATA:
        loadCsvFile(appData);
        break;
    case INPUT_REGION:
        setRegion(appData, params->region);
        break;
    case INPUT_COLUMN:
        setColumn(appData, params->column);
        break;
    case CALC_METRICS:
        calcMetrics(appData);
        break;
    }
}
