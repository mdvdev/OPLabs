#ifndef GRAPHWINDOW_H
#define GRAPHWINDOW_H

#include <QDialog>

class ScalablePixmapLabel;
struct AppData;

namespace Ui {
class GraphWindow;
}

class GraphWindow : public QDialog
{
    Q_OBJECT
private:
    Ui::GraphWindow* ui;
    ScalablePixmapLabel* graphLabel;
    AppData* appData;
public:
    explicit GraphWindow(AppData* appData, QWidget* parent = nullptr);
    ~GraphWindow();
    QPixmap createPixmap();
private:
    bool drawOx(QPixmap& pixmap, int xMin, int xMax, double yMin, double yMax);
    bool drawOy(QPixmap& pixmap, int xMin, int xMax, double yMin, double yMax);
    bool drawLines(QPixmap& pixmap, int xMin, int xMax, double yMin, double yMax);
    void showGraphLabel();
};

#endif // GRAPHWINDOW_H
