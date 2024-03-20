#include <QFileDialog>
#include <QStringList>

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Presenter.h"
#include "CsvParser.h"

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
}

MainWindow::~MainWindow()
{
    delete ui;
    destructCollection(appData.records);
    destructString(&appData.fileName);
    destructString(&appData.region);
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
    doOperation(LOAD_DATA, &appData,  NULL);
    updateUi();
}

void MainWindow::updateUi()
{
    updateFileNameLabel();
    updateTableWidget();
}

void MainWindow::updateFileNameLabel()
{
    ui->fileNameLineEdit->setText(appData.fileName.begin);
}

void MainWindow::updateTableWidget()
{
    if (appData.error != NO_ERROR || !appData.records) {
        return;
    }

    const CsvRecord* csvHeader = getRecordCsvRecordCollection(appData.records, 0);
    size_t columnCount = sizeCsvRecord(csvHeader);
    size_t rowCount = sizeCsvRecordCollection(appData.records);

    ui->tableWidget->setColumnCount(columnCount);
    ui->tableWidget->setRowCount(rowCount);
    addColumnHeaders(csvHeader, columnCount);

    size_t errorCount = fillTableWidget();

    //QWidget* window = new QWidget();
    //window->resize(320, 240);
    //window->show();
}

bool MainWindow::isValidCsvRecord(const CsvRecord* record, size_t columnCount)
{
    return sizeCsvRecord(record) == columnCount;
}

void MainWindow::addColumnHeaders(const CsvRecord* csvHeader, size_t columnCount)
{
    QStringList headers;

    for (size_t i = 0; i < columnCount; ++i) {
        headers << getFieldCsvRecord(csvHeader, i);
    }

    ui->tableWidget->setHorizontalHeaderLabels(headers);
}

size_t MainWindow::fillTableWidget()
{
    const CsvRecord* csvHeader = getRecordCsvRecordCollection(appData.records, 0);
    size_t columnCount = sizeCsvRecord(csvHeader);
    size_t collectionSize = sizeCsvRecordCollection(appData.records);
    size_t errorCount = 0;

    for (size_t i = 1; i < collectionSize; ++i) {
        const CsvRecord* record = getRecordCsvRecordCollection(appData.records, i);
        const char* region = getFieldCsvRecord(record, 1); // field 1 associated with region
        if (!isValidCsvRecord(record, columnCount)) {
            errorCount++;
        } else if (!isRecordContainsEmptyField(record) &&
                   (strcmp(appData.region.begin, "") == 0 || strcmp(appData.region.begin, region) == 0))
        {
            for (size_t j = 0; j < columnCount; ++j) {
                const char* field = getFieldCsvRecord(record, j);
                QTableWidgetItem* item = new QTableWidgetItem(field);
                ui->tableWidget->setItem(i-1, j, item);
            }
        }
    }

    return errorCount;
}

bool MainWindow::isRecordContainsEmptyField(const CsvRecord* record)
{
    size_t size = sizeCsvRecord(record);

    for (size_t i = 0; i < size; ++i) {
        if (!getFieldCsvRecord(record, i)) {
            return true;
        }
    }

    return false;
}
