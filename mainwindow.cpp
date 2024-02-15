#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "controller.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->outputLine->setReadOnly(true);

    initAppData();

    connect(ui->inputLine, &QLineEdit::textChanged, this, &MainWindow::inputLineChanged);
    connect(ui->convertButton, &QPushButton::clicked, this, &MainWindow::onConvertClicked);
    connect(ui->inputRadixComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::inputRadixSelected);
    connect(ui->outputRadixComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::outputRadixSelected);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::configureConvertButton()
{
    if (appData.error == NoError || appData.error == EmptyString)
        enableConvertButton();
    else
        disableConvertButton();
}

void MainWindow::onConvertClicked()
{
    std::string tempString = ui->inputLine->text().toStdString();
    AppParams params = { .input = tempString.c_str() };
    processInput(Convert, &appData, &params);
    configureConvertButton();
    updateLabels();
}

void MainWindow::inputRadixSelected(int index)
{
    AppParams params = { .radix = convertIndexToRadix(index) };
    processInput(SelectInputRadix, &appData, &params);
    configureConvertButton();
    updateLabels();
}

void MainWindow::outputRadixSelected(int index)
{
    AppParams params = { .radix = convertIndexToRadix(index) };
    processInput(SelectOutputRadix, &appData, &params);
    configureConvertButton();
    updateLabels();
}

void MainWindow::inputLineChanged()
{
    std::string tempString = ui->inputLine->text().toStdString();
    AppParams params = { .input = tempString.c_str() };
    processInput(EditInputLine, &appData, &params);
    configureConvertButton();
    updateLabels();
}

void MainWindow::updateErrorLabel()
{
    ui->errorLabel->setText(errorCodeToString());
}

void MainWindow::updateOutputLine()
{
    ui->outputLine->setText(appData.output);
}

void MainWindow::disableConvertButton()
{
    ui->convertButton->setEnabled(false);
}

void MainWindow::enableConvertButton()
{
    ui->convertButton->setEnabled(true);
}

// TODO implement through enums
int MainWindow::convertIndexToRadix(int index)
{
    switch (index) {
    case 0:
        return 2;
    case 1:
        return 8;
    default:
        return 10;
    }
}

void MainWindow::initAppData()
{
    appData.inputRadix = convertIndexToRadix(ui->inputRadixComboBox->currentIndex());
    appData.outputRadix = convertIndexToRadix(ui->outputRadixComboBox->currentIndex());
    appData.input[0] = '\0';
    appData.output[0] = '\0';
    appData.error = NoError;
}

void MainWindow::updateLabels()
{
    updateErrorLabel();
    updateOutputLine();
}

const char* MainWindow::errorCodeToString()
{
    switch (appData.error) {
    case NoError:
    case EmptyString:
        return "No Errors";
    case OutOfRange:
        return "Out of Range";
    case InvalidChar:
        return "Invalid Char";
    }
}
