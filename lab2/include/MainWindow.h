#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "AppData.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
    Ui::MainWindow* ui;
    AppData appData;
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
private slots:
    void onOpenFilePushButtonClicked();
    void onLoadPushButtonClicked();
private:
    void updateUi();
    void updateFileNameLabel();
    void updateTableWidget();

    void addColumnHeaders(const CsvRecord* csvHeader, size_t columnCount);
    size_t fillTableWidget();

    bool isRecordContainsEmptyField(const CsvRecord* record);
    bool isValidCsvRecord(const CsvRecord* record, size_t columnCount);
};

#endif // MAINWINDOW_H
