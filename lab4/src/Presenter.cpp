#include "Presenter.h"
#include "Model.h"

#define SCALE_MINUS_FACTOR 0.99
#define SCALE_PLUS_FACTOR  1.01

#define UP_MOVE_FACTOR 	  	 1
#define DOWN_MOVE_FACTOR  	-1
#define LEFT_MOVE_FACTOR  	-1
#define RIGHT_MOVE_FACTOR 	 1
#define BACK_MOVE_FACTOR  	 1
#define FORWARD_MOVE_FACTOR -1

#define XCW_ROTATE_FACTOR    1
#define XCCW_ROTATE_FACTOR  -1
#define YCW_ROTATE_FACTOR    1
#define YCCW_ROTATE_FACTOR  -1
#define ZCW_ROTATE_FACTOR   -1
#define ZCCW_ROTATE_FACTOR   1

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
    case SCALE_MINUS:
        scaleShape(appData, SCALE_MINUS_FACTOR);
        break;
    case SCALE_PLUS:
        scaleShape(appData, SCALE_PLUS_FACTOR);
        break;
    case UP_MOVEMENT:
        moveShape(appData, "Y", UP_MOVE_FACTOR);
        break;
    case DOWN_MOVEMENT:
        moveShape(appData, "Y", DOWN_MOVE_FACTOR);
        break;
    case LEFT_MOVEMENT:
        moveShape(appData, "X", LEFT_MOVE_FACTOR);
        break;
    case RIGHT_MOVEMENT:
        moveShape(appData, "X", RIGHT_MOVE_FACTOR);
        break;
    case BACK_MOVEMENT:
        moveShape(appData, "Z", BACK_MOVE_FACTOR);
        break;
    case FORWARD_MOVEMENT:
        moveShape(appData, "Z", FORWARD_MOVE_FACTOR);
        break;
    case XCW_ROTATION:
        rotateShape(appData, "X", XCW_ROTATE_FACTOR);
        break;
    case XCCW_ROTATION:
        rotateShape(appData, "X", XCCW_ROTATE_FACTOR);
        break;
    case YCW_ROTATION:
        rotateShape(appData, "Y", YCW_ROTATE_FACTOR);
        break;
    case YCCW_ROTATION:
        rotateShape(appData, "Y", YCCW_ROTATE_FACTOR);
        break;
    case ZCW_ROTATION:
        rotateShape(appData, "Z", ZCW_ROTATE_FACTOR);
        break;
    case ZCCW_ROTATION:
        rotateShape(appData, "Z", ZCCW_ROTATE_FACTOR);
        break;
    case NORMALIZE_SHAPE:
        normalizeShape(appData, params->normalizationFrom, params->normalizationTo);
        break;
    }
}
