#include <QPainter>

#include "GraphWindow.h"
#include "ScalablePixmapLabel.h"

ScalablePixmapLabel::ScalablePixmapLabel(GraphWindow* parent)
    : QLabel(parent)
    , parent(parent)
{
    setMinimumSize(1,1);
    setScaledContents(false);
}

void ScalablePixmapLabel::setPixmap(const QPixmap& p)
{
    pix = p;
    QLabel::setPixmap(pix);
}

void ScalablePixmapLabel::resizeEvent(QResizeEvent* event)
{
    if(!pix.isNull()) {
        QLabel::setPixmap(parent->createPixmap());
        QLabel::resizeEvent(event);
    }
}
