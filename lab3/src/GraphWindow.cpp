#include <QPainter>

#include "GraphWindow.h"
#include "ui_GraphWindow.h"
#include "ScalablePixmapLabel.h"

#include "AppData.h"

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
bool GraphWindow::drawOx(QPixmap& pixmap, int xMin, int xMax, double yMin, double yMax)
{
    const int width = graphLabel->width();
    const int height = graphLabel->height();
    const int scaleOx = width / (xMax - xMin);
    const double scaleOy = (double) height / (yMax - yMin);

    QPainter painter(&pixmap);

    const int oxShift = yMin < 0 ? height + scaleOy * yMin : height - 1;
    painter.drawLine(QPoint{0, oxShift}, QPoint{width, oxShift});
    painter.drawText(QPoint{width - 10, oxShift - 15}, "x");

    QPen pen;
    pen.setWidth(3);
    pen.setColor(Qt::black);
    painter.setPen(pen);

    QFont font = painter.font();
    font.setPointSize(6);
    painter.setFont(font);

    QFontMetrics fm(font);

    int collectionSize = sizeCsvRecordCollection(appData->records);

    for (int i = 1; i < collectionSize; ++i) {
        CsvRecord* record = getRecordCsvRecordCollection(appData->records, i);
        const char* argStr;
        int arg;

        if (strcmp(getFieldCsvRecord(record, 1), appData->region.begin)) {
            continue;
        }
        if (!(argStr = getFieldCsvRecord(record, 0))) {
            continue;
        }
        if (sscanf(argStr, "%d", &arg) != 1) {
            return false;
        }

        int x = scaleOx * (arg - xMin);
        int fontWidth = fm.horizontalAdvance(argStr);
        if (x + fontWidth >= width) {
            painter.drawText(QPoint{width - fontWidth, oxShift - 12}, argStr);
        } else {
            painter.drawText(QPoint{x, oxShift - 5}, argStr);
        }

        painter.drawPoint(QPoint{x, oxShift});
    }

    return true;
}

bool GraphWindow::drawOy(QPixmap& pixmap, int xMin, int xMax, double yMin, double yMax)
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

    QPainter painter(&pixmap);

    painter.drawLine(QPoint{0, 0}, QPoint{0, height});
    painter.drawText(QPoint{10, 20}, "y");

    QPen pen;
    pen.setWidth(3);
    pen.setColor(Qt::black);
    painter.setPen(pen);

    QFont font = painter.font();
    font.setPointSize(6);
    painter.setFont(font);

    QFontMetrics fm(font);

    int collectionSize = sizeCsvRecordCollection(appData->records);

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

        int x = scaleOx * (arg - xMin);
        int y = height - scaleOy * (value - yMin);
        int fontWidth = fm.horizontalAdvance(valueStr);

        pen.setColor(value == yMin || value == yMax || value == median ? Qt::red : Qt::black);
        painter.setPen(pen);

        if (x + fontWidth >= width) {
            painter.drawText(QPoint{width - fontWidth, y == 0 ? y + 10 : y == width ? y - 5 : y}, valueStr);
        } else {
            painter.drawText(QPoint{x + 3, y == 0 ? y + 10 : y == width ? y - 5 : y}, valueStr);
        }

        painter.drawPoint(QPoint{x, y});
    }

    return true;
}

bool GraphWindow::drawLines(QPixmap& pixmap, int xMin, int xMax, double yMin, double yMax)
{
    int column;

    if (sscanf(appData->column.begin, "%d", &column) != 1) {
        return false;
    }

    const int width = graphLabel->width();
    const int height = graphLabel->height();
    const int scaleOx = width / (xMax - xMin);
    const double scaleOy = (double) height / (yMax - yMin);

    QPainter painter(&pixmap);
    painter.setPen(Qt::black);

    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::black);
    painter.setPen(pen);

    int collectionSize = sizeCsvRecordCollection(appData->records);
    int saved_x = -1;
    int saved_y = -1;

    for (int i = 1; i < collectionSize; ++i) {
        CsvRecord* record = getRecordCsvRecordCollection(appData->records, i);
        const char* arg_str;
        const char* value_str;
        int arg;
        double value;
        if (strcmp(getFieldCsvRecord(record, 1), appData->region.begin)) {
            continue;
        }
        if (!(arg_str = getFieldCsvRecord(record, 0)) || !(value_str = getFieldCsvRecord(record, column - 1))) {
            continue;
        }
        if (sscanf(arg_str, "%d", &arg) != 1 || sscanf(value_str, "%lf", &value) != 1) {
            return false;
        }

        int x = scaleOx * (arg - xMin);
        int y = height - scaleOy * (value - yMin);
        if (i != 1) {
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

    if (!drawOx(pixmap, xMin, xMax, yMin, yMax) ||
        !drawOy(pixmap, xMin, xMax, yMin, yMax) ||
        !drawLines(pixmap, xMin, xMax, yMin, yMax))
    {
        return QPixmap();
    }

    pixmap.scaled(width, height, Qt::KeepAspectRatio);

    return pixmap;
}
