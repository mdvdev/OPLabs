#include <QFileDialog>
#include <QStringList>
#include <QMessageBox>
#include <QPainter>
#include <QGraphicsEllipseItem>

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Presenter.h"
#include "Point.h"
#include "List.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->fileNameLineEdit->setReadOnly(true);

    doOperation(INIT, &appData, NULL);
    updateUi();

    connect(ui->openFilePushButton, &QPushButton::clicked, this, &MainWindow::onOpenFilePushButtonClicked);
    connect(ui->loadDataPushButton, &QPushButton::clicked, this, &MainWindow::onLoadPushButtonClicked);

    connect(ui->minusScalingPushButton, &QPushButton::clicked, this, &MainWindow::onMinusScalingPushButtonClicked);
    connect(ui->plusScalingPushButton, &QPushButton::clicked, this, &MainWindow::onPlusScalingPushButtonClicked);

    connect(ui->upMovementPushButton, &QPushButton::clicked, this, &MainWindow::onUpMovementPushButtonClicked);
    connect(ui->downMovementPushButton, &QPushButton::clicked, this, &MainWindow::onDownMovementPushButtonClicked);
    connect(ui->leftMovementPushButton, &QPushButton::clicked, this, &MainWindow::onLeftMovementPushButtonClicked);
    connect(ui->rightMovementPushButton, &QPushButton::clicked, this, &MainWindow::onRightMovementPushButtonClicked);
    connect(ui->backMovementPushButton, &QPushButton::clicked, this, &MainWindow::onBackMovementPushButtonClicked);
    connect(ui->forwardMovementPushButton, &QPushButton::clicked, this, &MainWindow::onForwardMovementPushButtonClicked);

    connect(ui->xCWRotationPushButton, &QPushButton::clicked, this, &MainWindow::onXCWRotationPushButtonClicked);
    connect(ui->xCCWRotationPushButton, &QPushButton::clicked, this, &MainWindow::onXCCWRotationPushButtonClicked);
    connect(ui->yCWRotationPushButton, &QPushButton::clicked, this, &MainWindow::onYCWRotationPushButtonClicked);
    connect(ui->yCCWRotationPushButton, &QPushButton::clicked, this, &MainWindow::onYCCWRotationPushButtonClicked);
    connect(ui->zCWRotationPushButton, &QPushButton::clicked, this, &MainWindow::onZCWRotationPushButtonClicked);
    connect(ui->zCCWRotationPushButton, &QPushButton::clicked, this, &MainWindow::onZCCWRotationPushButtonClicked);
}

MainWindow::~MainWindow()
{
    delete ui;

    destructList(appData.pointMatrix, free);

    destructString(&appData.errorCount);
    destructString(&appData.totalCount);
    destructString(&appData.validCount);
    destructString(&appData.fileName);
}

void MainWindow::onOpenFilePushButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "/home", tr("CSV files (*.csv)"));
    std::string tempString = fileName.toStdString();
    Params params = { .fileName = tempString.c_str() };
    doOperation(OPEN_FILE, &appData, &params);
    updateUi();
}

void MainWindow::onLoadPushButtonClicked()
{
    doOperation(LOAD_DATA, &appData, NULL);

    QString from = ui->fromNormalizationLineEdit->text();
    QString to = ui->toNormalizationLineEdit->text();
    std::string tempFrom = from.toStdString();
    std::string tempTo = to.toStdString();
    if (tempFrom != "" || tempTo != "") {
        Params params;
        params.normalizationFrom = tempFrom.c_str();
        params.normalizationTo = tempTo.c_str();
        doOperation(NORMALIZE_SHAPE, &appData, &params);
    }

    updateUi();
    showInfoMessageBox();
}

void MainWindow::onMinusScalingPushButtonClicked()
{
    doOperation(SCALE_MINUS, &appData, NULL);
    updateUi();
}

void MainWindow::onPlusScalingPushButtonClicked()
{
    doOperation(SCALE_PLUS, &appData, NULL);
    updateUi();
}

void MainWindow::onUpMovementPushButtonClicked()
{
    doOperation(UP_MOVEMENT, &appData, NULL);
    updateUi();
}

void MainWindow::onDownMovementPushButtonClicked()
{
    doOperation(DOWN_MOVEMENT, &appData, NULL);
    updateUi();
}

void MainWindow::onLeftMovementPushButtonClicked()
{
    doOperation(LEFT_MOVEMENT, &appData, NULL);
    updateUi();
}

void MainWindow::onRightMovementPushButtonClicked()
{
    doOperation(RIGHT_MOVEMENT, &appData, NULL);
    updateUi();
}

void MainWindow::onBackMovementPushButtonClicked()
{
    doOperation(BACK_MOVEMENT, &appData, NULL);
    updateUi();
}

void MainWindow::onForwardMovementPushButtonClicked()
{
    doOperation(FORWARD_MOVEMENT, &appData, NULL);
    updateUi();
}

void MainWindow::onXCWRotationPushButtonClicked()
{
    doOperation(XCW_ROTATION, &appData, NULL);
    updateUi();
}

void MainWindow::onXCCWRotationPushButtonClicked()
{
    doOperation(XCCW_ROTATION, &appData, NULL);
    updateUi();
}

void MainWindow::onYCWRotationPushButtonClicked()
{
    doOperation(YCW_ROTATION, &appData, NULL);
    updateUi();
}

void MainWindow::onYCCWRotationPushButtonClicked()
{
    doOperation(YCCW_ROTATION, &appData, NULL);
    updateUi();
}

void MainWindow::onZCWRotationPushButtonClicked()
{
    doOperation(ZCW_ROTATION, &appData, NULL);
    updateUi();
}

void MainWindow::onZCCWRotationPushButtonClicked()
{
    doOperation(ZCCW_ROTATION, &appData, NULL);
    updateUi();
}

void MainWindow::updateUi()
{
    updateFileNameLineEdit();
    updateErrorLineEdit();
    updateNormalizationFrom();
    updateNormalizationTo();
    updateShape();
}

void MainWindow::updateErrorLineEdit()
{
    ui->errorLineEdit->setText(getErrorMessage());
}

void MainWindow::updateShape()
{
    QGraphicsScene* scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    drawShape(scene);
}

void MainWindow::updateNormalizationFrom()
{
    ui->fromNormalizationLineEdit->setText(appData.normalizeFrom.begin);
}

void MainWindow::updateNormalizationTo()
{
    ui->toNormalizationLineEdit->setText(appData.normalizeTo.begin);
}

void MainWindow::drawShape(QGraphicsScene* scene)
{
    if (appData.error != NO_ERROR || !appData.pointMatrix) {
        return;
    }

    const float obsScreenDist = 50.0f;
    const int cellSize = 20;
    int size = sizeList(appData.pointMatrix);

    QGraphicsEllipseItem* centralPoint = new QGraphicsEllipseItem(0, 0, cellSize/4, cellSize/4);
    centralPoint->setBrush(QColorConstants::Svg::black);
    centralPoint->setPos(0, 0);
    scene->addItem(centralPoint);

    for (int i = 0; i < size; ++i) {
        List* record = (List*) getDataList(appData.pointMatrix, i);
        int recordSize = sizeList(record);
        for (int j = 0; j < recordSize; ++j) {
            Point* point = (Point*) getDataList(record, j);
            float xScreen = obsScreenDist * point->x / (point->z + obsScreenDist);
            float yScreen = obsScreenDist * point->y / (point->z + obsScreenDist);
            QGraphicsEllipseItem* graphPoint = new QGraphicsEllipseItem(0, 0, cellSize/4, cellSize/4);

            graphPoint->setBrush(QColorConstants::Svg::red);
            graphPoint->setPos(xScreen*cellSize - cellSize/8, -yScreen*cellSize - cellSize/8);
            scene->addItem(graphPoint);

            if (j != 0) {
                Point* leftPoint = (Point*) getDataList(record, j - 1);
                float xScreenLeftPoint = obsScreenDist * leftPoint->x / (leftPoint->z + obsScreenDist);
                float yScreenLeftPoint = obsScreenDist * leftPoint->y / (leftPoint->z + obsScreenDist);
                scene->addLine(xScreen*cellSize, -yScreen*cellSize, xScreenLeftPoint*cellSize, -yScreenLeftPoint*cellSize,
                               QPen(QColorConstants::Svg::red));
            }

            if (i != 0) {
                List* upperRecord = (List*) getDataList(appData.pointMatrix, i - 1);
                Point* upperPoint = (Point*) getDataList(upperRecord, j);
                float xScreenUpperPoint = obsScreenDist * upperPoint->x / (upperPoint->z + obsScreenDist);
                float yScreenUpperPoint = obsScreenDist * upperPoint->y / (upperPoint->z + obsScreenDist);
                scene->addLine(xScreen*cellSize, -yScreen*cellSize, xScreenUpperPoint*cellSize, -yScreenUpperPoint*cellSize,
                               QPen(QColorConstants::Svg::red));
            }
        }
    }
}

const char* MainWindow::getErrorMessage()
{
    switch (appData.error) {
    case NO_ERROR:
        return "No errors";
    case OPEN_FILE_ERROR:
        return "Cannot open file";
    case PARSE_CSV_ERROR:
        return "Cannot parse CSV file (system error)";
    case INVALID_RECORD_ERROR:
        return "CSV file contains invalid record";
    case INVALID_RANGE_ERROR:
        return "Invalid range";
    case SKIP_RECORD_ERROR:
        return "";
    }
}

void MainWindow::updateFileNameLineEdit()
{
    ui->fileNameLineEdit->setText(appData.fileName.begin);
}

void MainWindow::showInfoMessageBox()
{
    QString str;
    str = QString("Errors: %1\nTotal: %2\nValid: %3")
              .arg(appData.errorCount.begin)
              .arg(appData.totalCount.begin)
              .arg(appData.validCount.begin);

    QMessageBox::information(this, "Info", str, QMessageBox::Ok);
}
