#include <QPainter>

#include "GraphWindow.h"
#include "ui_GraphWindow.h"
#include "ScalablePixmapLabel.h"

#include "AppData.h"

struct GraphParams {
    CsvRecord* record;
    QPainter* painter;
    int xMax, xMin;
    double yMax, yMin;
    double median;
    int width, height;
    int scaleOx;
    int column;
    double scaleOy;
    int oxShift;
    int arg;
    double value;
};

GraphWindow::GraphWindow(AppData* appData, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::GraphWindow)
    , appData(appData)
{
    ui->setupUi(this);
    setWindowTitle("Function Graph");

    graphLabel = new ScalablePixmapLabel(this);
    graphLabel->setObjectName("graphLabel");
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(graphLabel->sizePolicy().hasHeightForWidth());
    graphLabel->setSizePolicy(sizePolicy);

    graphLabel->setPixmap(createPixmap());

    ui->gridLayout->addWidget(graphLabel, 0, 0, 1, 1);
}

GraphWindow::~GraphWindow()
{
    delete ui;
}

void GraphWindow::showGraphLabel()
{
    if (!appData->records ||
        strcmp(appData->region.begin, "") == 0 ||
        strcmp(appData->column.begin, "") == 0 ||
        strcmp(appData->min.begin, "") == 0 ||
        strcmp(appData->max.begin, "") == 0 ||
        strcmp(appData->median.begin, "") == 0)
    {
        return;
    }

    graphLabel->setPixmap(createPixmap());

}

void GraphWindow::drawAxis(GraphParams params)
{
    QFont font = params.painter->font();
    font.setPointSize(6);
    QFontMetrics fm(font);
    int x = params.scaleOx * (params.arg - params.xMin);
    int y = params.height - params.scaleOy * (params.value - params.yMin);
    int fontWidth = fm.horizontalAdvance(QString::number(params.value));
    QPen pen;

    pen.setWidth(3);
    pen.setColor(params.value == params.yMin || params.value == params.yMax || params.value == params.median ?
                     Qt::red : Qt::black);
    params.painter->setPen(pen);

    // work with OX
    if (x + fontWidth >= params.width) {
        params.painter->drawText(QPoint{params.width - fontWidth, params.oxShift - 12},
                                 QString::number(params.arg));
    } else {
        params.painter->drawText(QPoint{x, params.oxShift - 5}, QString::number(params.arg));
    }

    params.painter->drawPoint(QPoint{x, params.oxShift});

    // work with OY
    if (x + fontWidth >= params.width) {
        params.painter->drawText(QPoint{params.width - fontWidth, y == 0 ?
                        y + 10 : y == params.width ? y - 5 : y}, QString::number(params.value));
    } else {
        params.painter->drawText(QPoint{x + 3, y == 0 ?
                        y + 10 : y == params.width ? y - 5 : y}, QString::number(params.value));
    }

    params.painter->drawPoint(QPoint{x, y});
}

bool GraphWindow::drawGraph(QPixmap& pixmap, int xMin, int xMax, double yMin, double yMax)
{
    int column;
    double median;

    if (sscanf(appData->column.begin, "%d", &column) != 1 ||
        sscanf(appData->median.begin, "%lf", &median) != 1)
    {
        return false;
    }

    const int width = graphLabel->width();
    const int height = graphLabel->height();
    const int scaleOx = width / (xMax - xMin);
    const double scaleOy = (double) height / (yMax - yMin);
    const int oxShift = yMin < 0 ? height + scaleOy * yMin : height - 1;

    QPainter painter(&pixmap);
    QPen pen;

    painter.drawLine(QPoint{0, oxShift}, QPoint{width, oxShift});
    painter.drawText(QPoint{width - 10, oxShift - 15}, "x");

    painter.drawLine(QPoint{0, 0}, QPoint{0, height});
    painter.drawText(QPoint{10, 20}, "y");

    int collectionSize = sizeCsvRecordCollection(appData->records);
    int saved_x = -1;
    int saved_y = -1;

    for (int i = 1; i < collectionSize; ++i) {
        CsvRecord* record = getRecordCsvRecordCollection(appData->records, i);
        const char* argStr;
        const char* valueStr;
        int arg;
        double value;
        if (strcmp(getFieldCsvRecord(record, 1), appData->region.begin)) {
            continue;
        }
        if (!(argStr = getFieldCsvRecord(record, 0)) || !(valueStr = getFieldCsvRecord(record, column - 1))) {
            continue;
        }
        if (sscanf(argStr, "%d", &arg) != 1 || sscanf(valueStr, "%lf", &value) != 1) {
            return false;
        }

        drawAxis(GraphParams{
                    .record = record,
                    .painter = &painter,
                    .xMax = xMax,
                    .xMin = xMin,
                    .yMin = yMin,
                    .yMax = yMax,
                    .median = median,
                    .width = width,
                    .height = height,
                    .scaleOx = scaleOx,
                    .scaleOy = scaleOy,
                    .column = column,
                    .oxShift = oxShift,
                    .arg = arg,
                    .value = value});

        int x = scaleOx * (arg - xMin);
        int y = height - scaleOy * (value - yMin);

        if (i != 1) {
            pen.setColor(Qt::black);
            pen.setWidth(1);
            painter.setPen(pen);
            painter.drawLine(QPoint{saved_x, saved_y}, QPoint{x, y});
        }

        saved_x = x;
        saved_y = y;
    }

    return true;
}

QPixmap GraphWindow::createPixmap()
{
    const char* xMinStr = getFieldCsvRecord(getRecordCsvRecordCollection(appData->records, 1), 0);
    const char* xMaxStr = getFieldCsvRecord(getRecordCsvRecordCollection(
                                                appData->records, sizeCsvRecordCollection(appData->records) - 1), 0);
    const char* yMinStr = appData->min.begin;
    const char* yMaxStr = appData->max.begin;

    int xMin, xMax;
    double yMin, yMax;

    if (sscanf(xMinStr, "%d", &xMin) != 1 ||
        sscanf(xMaxStr, "%d", &xMax) != 1 ||
        sscanf(yMinStr, "%lf", &yMin) != 1 ||
        sscanf(yMaxStr, "%lf", &yMax) != 1)
    {
        return QPixmap();
    }

    const int width = graphLabel->width();
    const int height = graphLabel->height();

    QPixmap pixmap(width, height);
    pixmap.fill(Qt::white);

    if (!drawGraph(pixmap, xMin, xMax, yMin, yMax)) {
        return QPixmap();
    }

    pixmap.scaled(width, height, Qt::KeepAspectRatio);

    return pixmap;
}
