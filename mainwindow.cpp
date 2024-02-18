#include <QClipboard>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "controller.h"
#include "lib.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->outputLine->setReadOnly(true);

    initAppData();

    disableConvertButton();
    disableSwapButton();

    connect(ui->inputLine, &QLineEdit::textChanged, this, &MainWindow::inputLineChanged);
    connect(ui->convertButton, &QPushButton::clicked, this, &MainWindow::onConvertClicked);
    connect(ui->swapButton, &QPushButton::clicked, this, &MainWindow::onSwapClicked);
    connect(ui->copyButton, &QPushButton::clicked, this, &MainWindow::onCopyClicked);
    connect(ui->inputRadixComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::inputRadixSelected);
    connect(ui->outputRadixComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::outputRadixSelected);
}

MainWindow::~MainWindow()
{
    delete ui;
    destructCharVector(&appData.input);
}

int MainWindow::radixArray[] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

void MainWindow::configureSwapButton()
{
    if (appData.error != NoError || isEmptyString(appData.output)) {
        disableSwapButton();
    } else {
        enableSwapButton();
    }
}

void MainWindow::configureConvertButton()
{
    if (appData.error == NoError) {
        enableConvertButton();
    } else {
        disableConvertButton();
    }
}

void MainWindow::configureButtons()
{
    configureConvertButton();
    configureSwapButton();
}

void MainWindow::onConvertClicked()
{
    processInput(Convert, &appData, NULL);
    configureButtons();
    updateLabels();
}

void MainWindow::onSwapClicked()
{
    processInput(Swap, &appData, NULL);
    configureButtons();
    updateLabels();
}

void MainWindow::onCopyClicked()
{
    processInput(CopyOutput, &appData, NULL);
    QClipboard* clip = QApplication::clipboard();
    clip->setText(appData.output);
    configureButtons();
    updateLabels();
}

void MainWindow::inputRadixSelected(int index)
{
    AppParams params = { .radix = convertIndexToRadix(index) };
    processInput(SelectInputRadix, &appData, &params);
    configureButtons();
    updateLabels();
}

void MainWindow::outputRadixSelected(int index)
{
    AppParams params = { .radix = convertIndexToRadix(index) };
    processInput(SelectOutputRadix, &appData, &params);
    configureButtons();
    updateLabels();
}

void MainWindow::inputLineChanged()
{
    std::string tempString = ui->inputLine->text().toStdString();
    AppParams params = { .input = tempString.c_str() };
    processInput(EditInputLine, &appData, &params);
    configureButtons();
    updateLabels();
}

void MainWindow::updateErrorLabel()
{
    ui->errorLabel->setText(errorCodeToString());
}

void MainWindow::updateInputLine()
{
    ui->inputLine->setText(appData.input.begin);
}

void MainWindow::updateOutputLine()
{
    ui->outputLine->setText(appData.output);
}

void MainWindow::updateInputRadix()
{
    ui->inputRadixComboBox->setCurrentIndex(convertRadixToIndex(appData.inputRadix));
}

void MainWindow::updateOutputRadix()
{
    ui->outputRadixComboBox->setCurrentIndex(convertRadixToIndex(appData.outputRadix));
}

void MainWindow::disableConvertButton()
{
    ui->convertButton->setEnabled(false);
}

void MainWindow::enableConvertButton()
{
    ui->convertButton->setEnabled(true);
}

void MainWindow::disableSwapButton()
{
    ui->swapButton->setEnabled(false);
}

void MainWindow::enableSwapButton()
{
    ui->swapButton->setEnabled(true);
}

int MainWindow::convertIndexToRadix(int index)
{
    const int arraySize = sizeof(radixArray)/sizeof(radixArray[0]);
    return index >= 0 && index < arraySize ?
                radixArray[index] : -1;
}

int MainWindow::convertRadixToIndex(int radix)
{
    const int arraySize = sizeof(radixArray)/sizeof(radixArray[0]);
    for (int i = 0; i < arraySize; ++i) {
        if (radixArray[i] == radix) {
            return i;
        }
    }
    return -1;
}

void MainWindow::initAppData()
{
    appData.inputRadix = convertIndexToRadix(ui->inputRadixComboBox->currentIndex());
    appData.outputRadix = convertIndexToRadix(ui->outputRadixComboBox->currentIndex());
    constructCharVector(&appData.input, "");
    appData.output[0] = '\0';
    appData.error = NoError;
}

void MainWindow::updateLabels()
{
    updateInputRadix();
    updateOutputRadix();
    updateErrorLabel();
    updateInputLine();
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
