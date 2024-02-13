#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "controller.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->outputLine->setReadOnly(true);

    appData.inputRadix = convertIndexToRadix(ui->inputRadixComboBox->currentIndex());
    appData.outputRadix = convertIndexToRadix(ui->outputRadixComboBox->currentIndex());

    connect(ui->inputLine, &QLineEdit::textChanged, this, &MainWindow::inputLineChanged);
    connect(ui->convertButton, &QPushButton::clicked, this, &MainWindow::onConvertClicked);
    connect(ui->inputRadixComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::inputRadixSelected);
    connect(ui->outputRadixComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::outputRadixSelected);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onConvertClicked()
{
    std::string tempString = ui->inputLine->text().toStdString();
    AppParams params = { .mainWindow = this, .input = tempString.c_str() };
    processConvertClick(&appData, &params);
}

void MainWindow::inputRadixSelected(int index)
{
    AppParams params = { .mainWindow = this, .radix = convertIndexToRadix(index) };
    processInputRadixSelect(&appData, &params);
}

void MainWindow::outputRadixSelected(int index)
{
    AppParams params = { .mainWindow = this, .radix = convertIndexToRadix(index) };
    processOutputRadixSelect(&appData, &params);
}

void MainWindow::inputLineChanged()
{
    std::string tempString = ui->inputLine->text().toStdString();
    AppParams params = { .mainWindow = this, .input = tempString.c_str() };
    processChangedInputLine(&appData, &params);
}

void MainWindow::updateErrorLabel(const char* error)
{
    ui->errorLabel->setText(error);
}

void MainWindow::updateOutputLine(const char* output)
{
    ui->outputLine->setText(output);
}

void MainWindow::disableConvertButton()
{
    ui->convertButton->setEnabled(false);
}

void MainWindow::enableConvertButton()
{
    ui->convertButton->setEnabled(true);
}

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
