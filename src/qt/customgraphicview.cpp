#include "customgraphicview.h"
#include <QtGui/QResizeEvent>
#include <QtWidgets/QGraphicsScene>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtWidgets/QGraphicsTextItem>
#include <QDateTime>
#include <QtGui/QMouseEvent>
#include <QApplication>
#include <qmath.h>
#include <QTimeLine>
#include <iostream>
#include "callout.h"
#include "../Config.h"

CustomGraphicView::CustomGraphicView(const QString &title, const QString &dateformat, QWidget *parent)
    : QGraphicsView(new QGraphicsScene, parent),
      _coordX(0),
      _coordY(0),
      _axisX(new QtCharts::QDateTimeAxis()),
      _axisY(new QtCharts::QValueAxis()),
      _chart(0),
      _tooltip(0),
      _hoveredDate(""),
      _dateformat(dateformat),
      _numScheduledScalings(0)
{
    // chart
    _chart = new QChart();
    _chart->setTitle(title);
    _chart->setTheme(QtCharts::QChart::ChartThemeDark);
    _chart->addAxis(_axisX, Qt::AlignBottom);
    _chart->addAxis(_axisY, Qt::AlignLeft);
    _chart->setAcceptHoverEvents(true);

    setInteractive(true);
    setRenderHint(QPainter::Antialiasing);
    setMouseTracking(true);
    scene()->addItem(_chart);

    _coordX = new QGraphicsSimpleTextItem(_chart);
    _coordX->setPos(_chart->size().width() / 2 - 100, _chart->size().height());

    _coordY = new QGraphicsSimpleTextItem(_chart);
    _coordY->setPos(_chart->size().width() / 2 + 50, _chart->size().height());


}

QtCharts::QDateTimeAxis *CustomGraphicView::axisX()
{
    return _axisX;
}

QtCharts::QValueAxis *CustomGraphicView::axisY()
{
    return _axisY;
}

void CustomGraphicView::connectDataSeries(QLineSeries *series, int numTick)
{
    _chart->addSeries(series);
    _axisX->setTickCount(numTick);
    _axisX->setFormat("MMM.yy");
    _axisX->setTitleText("Date");
    series->attachAxis(_axisX);
    series->attachAxis(_axisY);
    _axisY->setLabelFormat("%i");
    setRenderHint(QPainter::Antialiasing);
    series->connect(series, &QLineSeries::clicked, this, &CustomGraphicView::keepCallout);
    connect(series, &QLineSeries::hovered, this, &CustomGraphicView::tooltip);
}

// Callout Implementation
void CustomGraphicView::resizeEvent(QResizeEvent *event)
{
    if (scene())
    {
        scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
        _chart->resize(event->size());
        _coordX->setPos(_chart->size().width() / 2 - 50, _chart->size().height() - 20);
        _coordY->setPos(_chart->size().width() / 2 + 50, _chart->size().height() - 20);
        const auto callouts = _callouts;
        for (Callout *callout : callouts)
            callout->updateGeometry();
    }
    QGraphicsView::resizeEvent(event);
}

void CustomGraphicView::mouseMoveEvent(QMouseEvent *event)
{
    _hoveredDate = QDateTime::fromMSecsSinceEpoch(_chart->mapToValue(event->pos()).x()).toString(_dateformat);
    // _coordX->setText(QString("Date: %1").arg(_hoveredDate));
    // _coordY->setText(QString("Value: %1").arg(_chart->mapToValue(event->pos()).y()));
    QGraphicsView::mouseMoveEvent(event);
}

void CustomGraphicView::keepCallout()
{
    _callouts.append(_tooltip);
    _tooltip = new Callout(_chart);
}

void CustomGraphicView::tooltip(QPointF point, bool state)
{
    if (_tooltip == 0)
        _tooltip = new Callout(_chart);

    if (state)
    {
        _tooltip->setText(QString("Date: %1 \nValue: %2 ").arg(_hoveredDate).arg(point.y()));
        _tooltip->setAnchor(point);
        _tooltip->setZValue(11);
        _tooltip->updateGeometry();
        _tooltip->show();
    }
    else
    {
        _tooltip->hide();
    }
}

// Zoom In/Out
// ------------
void CustomGraphicView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Plus:
        _chart->zoomIn();
        break;
    case Qt::Key_Minus:
        _chart->zoomOut();
        break;
    case Qt::Key_Left:
        _chart->scroll(-10, 0);
        break;
    case Qt::Key_Right:
        _chart->scroll(10, 0);
        break;
    case Qt::Key_Up:
        _chart->scroll(0, 10);
        break;
    case Qt::Key_Down:
        _chart->scroll(0, -10);
        break;
    default:
        QGraphicsView::keyPressEvent(event);
        break;
    }
}

void CustomGraphicView::wheelEvent(QWheelEvent *event)
{
    event->delta() > 0 ? _chart->zoomIn() : _chart->zoomOut();
}
