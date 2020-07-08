#ifndef APPGUI_H
#define APPGUI_H

#include <QFileDialog>
#include <QInputDialog>
#include <QLayout>
#include <QMainWindow>
#include <QMessageBox>
#include <QMutex>
#include <QStandardItemModel>
#include <QThread>
#include <QWaitCondition>
#include <QtCharts/QChartView>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QValueAxis>
#include <QtWidgets/QTableView>
#include <memory>
#include "../AppControl.h"
#include "../MsgQueue.h"
#include "customgraphicview.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class AppGui;
}
QT_END_NAMESPACE

class UpdaterThread : public QThread
{
    Q_OBJECT
    shared_ptr<AppControl> _app_control;
    bool _is_start;
    QMutex mutex;
    QWaitCondition condition;

signals:
    void updatedAsset(UpdateData upd_data);

protected:
    void run() override;

public:
    UpdaterThread() { _is_start = true; };
    explicit UpdaterThread(shared_ptr<AppControl> appCtrl,
                           QObject *parent = nullptr)
        : QThread(parent)
    {
        // _app_control = appCtrl;
        _is_start = true;
        _app_control = appCtrl;
    };
    ~UpdaterThread()
    {
        mutex.lock();
        condition.wakeOne();
        mutex.unlock();
        wait();
    };
    void setAppControl(shared_ptr<AppControl> appCtrl)
    {
        _app_control = appCtrl;
    };
    void restart() { _is_start = true; }
};

class AppGui : public QMainWindow
{
    Q_OBJECT

public:
    AppGui(QWidget *parent = nullptr);
    ~AppGui();

private slots:
    void on_actionNew_triggered();

    void on_actionExit_triggered();

    void on_actionInfo_triggered();

    void on_actionAddKey_triggered();

    void on_actionSave_triggered();

    void updateWatchlistModel(UpdateData upd_data);

    void on_actionOpen_triggered();

    void on_tbtnTransaction_clicked();

    void on_tbtnWatchlist_clicked();

private:
    Ui::AppGui *ui;
    QtCharts::QChart *_qchart;
    QtCharts::QPieSeries *_pieseries;
    QtCharts::QChartView *_chartView;
    CustomGraphicView *_roiChartView;
    // QtCharts::QDateTimeAxis *_axisX;
    // QtCharts::QValueAxis *_axisY;
    QtCharts::QLineSeries *_roi_date_series;
    QGridLayout layout;
    QGridLayout layout_roi;

    // Control variables
    shared_ptr<AppControl> _appControl;
    vector<string> _def_activity_columns;
    shared_ptr<QStandardItemModel> _transaction_model;
    shared_ptr<QStandardItemModel> _watchlist_model;
    UpdaterThread _updater;

    // internal function
    void initTvTransactions(unsigned int row, unsigned int col);
    void initWatchlistModel();
    void showMsgWindow(QMessageBox::Icon &&msgtype,
                       const std::string title,
                       const std::string msg);
    void closeEvent(QCloseEvent *event);

    void createPieChart(vector<string> &categories, vector<double> &data);
    void createRoiChart();
    void watchlistUpdater();
    void setWatchlistColor(float update_var,
                           uint rowidx,
                           uint colidx,
                           float threshold = 10.);
};
#endif // APPGUI_H
