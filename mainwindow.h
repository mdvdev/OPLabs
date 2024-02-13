#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "appdata.h"

class Presenter;

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

    void updateErrorLabel(const char* error);
    void updateOutputLine(const char* output);
    void disableConvertButton();
    void enableConvertButton();

private slots:
    void onConvertClicked();
    void outputRadixSelected(int radix);
    void inputRadixSelected(int radix);
    void inputLineChanged();

private:
    int convertIndexToRadix(int index);

    Ui::MainWindow* ui;
    AppData appData;
};
#endif // MAINWINDOW_H
