#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "appdata.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onConvertClicked();
    void onSwapClicked();
    void onCopyClicked();
    void outputRadixSelected(int radix);
    void inputRadixSelected(int radix);
    void inputLineChanged();

private:
    void updateLabels();
    void updateErrorLabel();
    void updateOutputLine();
    void updateInputLine();
    void updateInputRadix();
    void updateOutputRadix();

    void disableConvertButton();
    void enableConvertButton();
    void disableSwapButton();
    void enableSwapButton();

    void configureButtons();
    void configureSwapButton();
    void configureConvertButton();

    int convertIndexToRadix(int index);
    int convertRadixToIndex(int radix);

    const char* errorCodeToString();

    void initAppData();

    Ui::MainWindow* ui;
    AppData appData;
    static int radixArray[];
};
#endif // MAINWINDOW_H
