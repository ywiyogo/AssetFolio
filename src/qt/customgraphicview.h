#ifndef CUSTOMGRAPHICVIEW_H
#define CUSTOMGRAPHICVIEW_H
#include <QtWidgets/QGraphicsView>
#include <QtCharts/QChartGlobal>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QLineSeries>
#include <QObject>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QMouseEvent;
class QResizeEvent;
QT_END_NAMESPACE

class Callout;

QT_CHARTS_USE_NAMESPACE

class CustomGraphicView : public QGraphicsView
{
    Q_OBJECT

public:
    CustomGraphicView(const QString &title, const QString &dateformat, QWidget *parent = 0);
    void connectDataSeries(QLineSeries *series, int numTick);
    QtCharts::QDateTimeAxis *axisX();
    QtCharts::QValueAxis *axisY();

protected:
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);


public slots:
    void keepCallout();
    void tooltip(QPointF point, bool state);

private:
    QGraphicsSimpleTextItem *_coordX;
    QGraphicsSimpleTextItem *_coordY;
    QtCharts::QDateTimeAxis *_axisX;
    QtCharts::QValueAxis *_axisY;
    QChart *_chart;
    Callout *_tooltip;
    QList<Callout *> _callouts;
    QString _hoveredDate;
    QString _dateformat;
    int _numScheduledScalings;
};

#endif
