#ifndef GRAPHWINDOW_H
#define GRAPHWINDOW_H

#include <QDialog>

class ScalablePixmapLabel;
struct AppData;
struct GraphParams;

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
    void drawAxis(GraphParams params);
    bool drawGraph(QPixmap& pixmap, int xMin, int xMax, double yMin, double yMax);
    void showGraphLabel();
};

#endif // GRAPHWINDOW_H
