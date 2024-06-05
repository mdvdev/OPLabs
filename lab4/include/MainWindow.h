#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

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
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
private slots:
    void onOpenFilePushButtonClicked();
    void onLoadPushButtonClicked();

    void onMinusScalingPushButtonClicked();
    void onPlusScalingPushButtonClicked();

    void onUpMovementPushButtonClicked();
    void onDownMovementPushButtonClicked();
    void onLeftMovementPushButtonClicked();
    void onRightMovementPushButtonClicked();
    void onBackMovementPushButtonClicked();
    void onForwardMovementPushButtonClicked();

    void onXCWRotationPushButtonClicked();
    void onXCCWRotationPushButtonClicked();
    void onYCWRotationPushButtonClicked();
    void onYCCWRotationPushButtonClicked();
    void onZCWRotationPushButtonClicked();
    void onZCCWRotationPushButtonClicked();
private:
    void updateUi();
    void updateFileNameLineEdit();
    void updateErrorLineEdit();
    void updateShape();
    void updateNormalizationFrom();
    void updateNormalizationTo();

    void drawShape(QGraphicsScene* scene);

    const char* getErrorMessage();

    void showInfoMessageBox();
};

#endif // MAINWINDOW_H
