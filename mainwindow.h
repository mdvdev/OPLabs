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
    void outputRadixSelected(int radix);
    void inputRadixSelected(int radix);
    void inputLineChanged();

private:
    const char* errorCodeToString();
    void updateErrorLabel();
    void updateOutputLine();
    void disableConvertButton();
    void enableConvertButton();
    void configureConvertButton();

    int convertIndexToRadix(int index);
    void updateLabels();
    void initAppData();

    Ui::MainWindow* ui;
    AppData appData;
};
#endif // MAINWINDOW_H
