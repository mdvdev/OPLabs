#include <QFileDialog>
#include <QStringList>
#include <QMessageBox>

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Presenter.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->fileNameLineEdit->setReadOnly(true);
    ui->minLineEdit->setReadOnly(true);
    ui->maxLineEdit->setReadOnly(true);
    ui->medianLineEdit->setReadOnly(true);

    doOperation(INIT, &appData, NULL);
    updateUi();

    connect(ui->openFilePushButton, &QPushButton::clicked, this, &MainWindow::onOpenFilePushButtonClicked);
    connect(ui->loadDataPushButton, &QPushButton::clicked, this, &MainWindow::onLoadPushButtonClicked);
    connect(ui->calculateMetricsPushButton, &QPushButton::clicked, this, &MainWindow::onCalcMetricsPushButtonClicked);
    connect(ui->regionLineEdit, &QLineEdit::textChanged, this, &MainWindow::regionEntered);
    connect(ui->columnLineEdit, &QLineEdit::textChanged, this, &MainWindow::columnEntered);
}

MainWindow::~MainWindow()
{
    delete ui;

    destructCsvRecordCollection(appData.records);

    destructString(&appData.errorCount);

    destructString(&appData.totalCount);

    destructString(&appData.validCount);

    destructString(&appData.fileName);

    destructString(&appData.region);

    destructString(&appData.column);

    destructString(&appData.min);

    destructString(&appData.max);

    destructString(&appData.median);
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
    displayInfoMessageBox();
}

void MainWindow::onCalcMetricsPushButtonClicked()
{
    doOperation(CALC_METRICS, &appData, NULL);
    updateUi();
}

void MainWindow::regionEntered()
{
    QString region = ui->regionLineEdit->text();
    std::string tempRegion = region.toStdString();
    Params params = { .region = tempRegion.c_str() };
    doOperation(INPUT_REGION, &appData, &params);
    updateUi();
}

void MainWindow::columnEntered()
{
    QString column = ui->columnLineEdit->text();
    std::string tempColumn = column.toStdString();
    Params params = { .column = tempColumn.c_str() };
    doOperation(INPUT_COLUMN, &appData, &params);
    updateUi();
}

void MainWindow::updateUi()
{
    updateFileNameLineEdit();

    updateRegionLineEdit();

    updateColumnLineEdit();

    updateMinimumLineEdit();

    updateMaximumLineEdit();

    updateMedianLineEdit();

    updateTableWidget();

    updateErrorLineEdit();
}

void MainWindow::updateErrorLineEdit()
{
    ui->errorLineEdit->setText(getErrorMessage());
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
    case INVALID_COLUMN_ERROR:
        return "Entered invalid column No.";
    case INVALID_FIELD_ERROR:
        return "CSV contains invalid field";
    case REGION_NOT_EXIST:
        return "Region not exist";
    case SKIP_RECORD_ERROR:
        return "";
    }
}

void MainWindow::updateFileNameLineEdit()
{
    ui->fileNameLineEdit->setText(appData.fileName.begin);
}

void MainWindow::updateRegionLineEdit()
{
    ui->regionLineEdit->setText(appData.region.begin);
}

void MainWindow::updateColumnLineEdit()
{
    ui->columnLineEdit->setText(appData.column.begin);
}

void MainWindow::updateMinimumLineEdit()
{
    ui->minLineEdit->setText(appData.min.begin);
}

void MainWindow::updateMaximumLineEdit()
{
    ui->maxLineEdit->setText(appData.max.begin);
}

void MainWindow::updateMedianLineEdit()
{
    ui->medianLineEdit->setText(appData.median.begin);
}

void MainWindow::updateTableWidget()
{
    if (!appData.records) {
        return;
    }

    clearTableWidget();

    const CsvRecord* csvHeader = getRecordCsvRecordCollection(appData.records, 0);
    int columnCount = sizeCsvRecord(csvHeader);

    ui->tableWidget->setColumnCount(columnCount);
    addColumnHeaders(csvHeader, columnCount);

    fillTableWidget();
}

void MainWindow::displayInfoMessageBox()
{
    QString str;
    str = QString("Errors: %1\nTotal: %2\nValid: %3")
              .arg(appData.errorCount.begin)
              .arg(appData.totalCount.begin)
              .arg(appData.validCount.begin);

    QMessageBox::information(this, "Info", str, QMessageBox::Ok);
}

void MainWindow::clearTableWidget()
{
    ui->tableWidget->setRowCount(0);
}

void MainWindow::addColumnHeaders(const CsvRecord* csvHeader, int columnCount)
{
    QStringList headers;

    for (int i = 0; i < columnCount; ++i) {
        headers << getFieldCsvRecord(csvHeader, i);
    }

    ui->tableWidget->setHorizontalHeaderLabels(headers);
}

void MainWindow::fillTableWidget()
{
    const CsvRecord* csvHeader = getRecordCsvRecordCollection(appData.records, 0);
    int columnCount = sizeCsvRecord(csvHeader);
    int collectionSize = sizeCsvRecordCollection(appData.records);

    for (int i = 1; i < collectionSize; ++i) {
        const CsvRecord* record = getRecordCsvRecordCollection(appData.records, i);
        const char* region = getFieldCsvRecord(record, 1); // field 1 associated with region

        if (isValidCsvRecord(record, columnCount) &&
            !isRecordContainsEmptyField(record) &&
            (strcmp(appData.region.begin, "") == 0 || strcmp(appData.region.begin, region) == 0))
        {
            ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);

            for (int j = 0; j < columnCount; ++j) {
                const char* field = getFieldCsvRecord(record, j);
                QTableWidgetItem* item = new QTableWidgetItem(field);
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, j, item);
            }
        }
    }
}

int MainWindow::isRecordContainsEmptyField(const CsvRecord* record)
{
    int size = sizeCsvRecord(record);

    for (int i = 0; i < size; ++i) {
        if (!getFieldCsvRecord(record, i)) {
            return 1;
        }
    }

    return 0;
}
