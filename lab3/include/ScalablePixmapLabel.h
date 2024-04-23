#ifndef SCALABLEPIXMAPLABEL_H
#define SCALABLEPIXMAPLABEL_H

#include <QLabel>
#include <QObject>

struct AppData;
class GraphWindow;

class ScalablePixmapLabel : public QLabel {
    Q_OBJECT
private:
    QPixmap pix;
    GraphWindow* parent;
public:
    explicit ScalablePixmapLabel(GraphWindow* parent = nullptr);
public slots:
    void setPixmap(const QPixmap&);
    void resizeEvent(QResizeEvent*) override;
};

#endif // SCALABLEPIXMAPLABEL_H
