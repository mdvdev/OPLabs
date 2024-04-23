#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "AppData.h"

class ScalablePixmapLabel;

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
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    friend class ScalablePixmapLabel;
private slots:
    void onOpenFilePushButtonClicked();
    void onLoadPushButtonClicked();
    void onCalcDrawPushButtonClicked();
    void regionEntered();
    void columnEntered();
private:
    void updateUi();
    void updateFileNameLineEdit();
    void updateTableWidget();
    void updateRegionLineEdit();
    void updateColumnLineEdit();
    void updateMinimumLineEdit();
    void updateMaximumLineEdit();
    void updateMedianLineEdit();
    void updateErrorLineEdit();

    void showGraphWindow();

    const char* getErrorMessage();

    void showInfoMessageBox();

    void addColumnHeaders(const CsvRecord* csvHeader, int columnCount);
    void fillTableWidget();
    void clearTableWidget();

    int isRecordContainsEmptyField(const CsvRecord* record);
};

#endif // MAINWINDOW_H
