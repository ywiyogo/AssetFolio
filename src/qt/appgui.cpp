#include "appgui.h"
#include "../Config.h"
#include "ui_appgui.h"
#include <QMetaType>

AppGui::AppGui(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::AppGui),
      _qchart(new QtCharts::QChart()),
      _pieseries(new QtCharts::QPieSeries()),
      _chartView(new QtCharts::QChartView(_qchart)),
      _appControl(make_shared<AppControl>(Config::UPDATE_PERIODE)),
      _transaction_model(make_shared<QStandardItemModel>(
          0, Config::TRANSACTION_COL_NAMES.size(), this)),
      _updater()
{
    ui->setupUi(this);
    // init chart
    _qchart->setAnimationOptions(QtCharts::QChart::AllAnimations);
    _qchart->legend()->setAlignment(Qt::AlignRight);
    _chartView->setRenderHint(QPainter::Antialiasing);
    _chartView->chart()->setTheme(QtCharts::QChart::ChartThemeBlueCerulean);
    layout.addWidget(_chartView);
    ui->tab_alloc->setLayout(&layout);
}

AppGui::~AppGui() { delete ui; }

void AppGui::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open a JSON file",
                                                    ".", "Images (*.json)");
    if (filename.isNull())
    {
        return;
    }
    // Showing the JSON file in the spreadsheet
    bool isValid = false;
    // _appControl->clearJsonData();
    try
    {
        _appControl->clearJsonData();
        isValid = _appControl->readLocalRapidJson(
            filename.toStdString().c_str());
    }
    catch (const exception &e)
    {
        showMsgWindow(QMessageBox::Warning, "Exception Error", e.what());
    }
    // read and check the JSON file
    if (isValid)
    {
        shared_ptr<rapidjson::Document> jsonDoc = _appControl->getJsonDoc();

        auto json_entries = jsonDoc->GetObject()["Transactions"].GetArray();

        int rowPos = 0;
        int colPos = 0;
        int offset = 10;

        int numOfRows = json_entries.Size() + offset;

        initTvTransactions(numOfRows, json_entries[0].MemberCount());

        for (rapidjson::Value::ConstValueIterator itr = json_entries.Begin();
             itr != json_entries.End(); ++itr)
        {

            // creating the asset object
            rapidjson::Value::ConstMemberIterator itr2;
            for (itr2 = itr->MemberBegin(); itr2 != itr->MemberEnd(); ++itr2)
            {
                // if (rowPos == 0)
                // { // set the column label name
                //     ui->tableView->setHeader SetColLabelValue(
                //         colPos, QString(itr2->name.GetString()));
                // }
                QModelIndex index =
                    _transaction_model->index(rowPos, colPos, QModelIndex());
                if (itr2->value.IsString())
                {

                    // 0 for all data
                    _transaction_model->setData(
                        index, QString(itr2->value.GetString()));
                }
                else if (itr2->value.IsNumber())
                {
                    _transaction_model->setData(
                        index, QString(AppControl::floatToString(
                                           itr2->value.GetFloat(), 2)
                                           .c_str()));
                }
                else
                {
                    std::cout << "Label: " << itr2->name.GetString() << flush;
                    std::cout << "Value type is: " << itr2->value.GetType()
                              << endl;
                }
                colPos++;
            }
            rowPos++;
            colPos = 0;
        }
        // ui->tableView->horizontalHeader()->setSectionResizeMode(
        //     QHeaderView::Stretch);
        ui->tableView->resizeColumnsToContents();

        // Fill the watchlist viewer
        vector<string> colWatchlist = {
            "Name", "Amount", "Total transaktion", "Buying price",
            "Curr. Price", "Curr. Asset", "Allocation", "Changed %",
            "Change", "Yield %", "TotalYield"};

        // update the piechart
        vector<double> data;
        vector<string> categories;
        _appControl->calcAllocation(categories, data);
        createPieChart(categories, data);
    }
    else
    {
        showMsgWindow(QMessageBox::Warning, "Failure",
                      "JSON file is not valid.");
    }
}
void AppGui::on_actionNew_triggered()
{
    _appControl.reset(new AppControl(Config::UPDATE_PERIODE));
    _appControl->clearJsonData();
    initTvTransactions(10, Config::WATCHLIST_COL_NAMES.size());
}

void AppGui::on_actionExit_triggered() { QCoreApplication::quit(); }

void AppGui::on_actionInfo_triggered()
{
    showMsgWindow(
        QMessageBox::Information, "About Assetfolio",
        "An Asset Portfolio Tracker Application that keeps your asset data "
        "private. We don't need to signup and give up our data to the cloud "
        "server.\nCheck and read the README in "
        "https://github.com/ywiyogo/Assetfolio\n\nDeveloped by Yongkie Wiyogo, "
        "2020");
}

void AppGui::on_actionRefresh_triggered() {}

void AppGui::on_actionSave_triggered()
{
    QString filename = QFileDialog::getSaveFileName(
        this, "Save transactions to JSON file", ".", "JSON (*.json)");

    if (filename.isNull())
    {
        return;
    }

    // vector<string> col_names;
    shared_ptr<rapidjson::Document> json_save = _appControl->getJsonDoc();
    string savepath = filename.toStdString();
    string backuppath = savepath + ".bak";
    // save a backup
    _appControl->saveJson(backuppath);
    json_save->RemoveAllMembers();
    json_save->SetObject();
    rapidjson::Document::AllocatorType &allocator = json_save->GetAllocator();

    rapidjson::Value entry_array(rapidjson::kArrayType);

    for (int row = 0; row < _transaction_model->rowCount(); row++)
    {
        rapidjson::Value entry_obj(rapidjson::kObjectType);
        QModelIndex index = _transaction_model->index(row, 0, QModelIndex());
        cout << _transaction_model->data(index).toString().toStdString()
             << endl;
        if (_transaction_model->data(index).toString().toStdString().empty())
        { // first empty row
            continue;
        }

        for (uint col = 0; col < Config::TRANSACTION_COL_NAMES.size(); col++)
        {
            string column_name =
                _transaction_model->headerData(col, Qt::Horizontal)
                    .toString()
                    .toStdString();
            rapidjson::Value name(column_name.c_str(), allocator);
            rapidjson::Value value;
            index = _transaction_model->index(row, col, QModelIndex());
            if ((column_name.compare("Transaction") == 0) ||
                (column_name.compare("Amount") == 0))
            {

                value.SetFloat(AppControl::stringToFloat(
                    _transaction_model->data(index).toString().toStdString(),
                    2));
            }
            else
            {
                value.SetString(_transaction_model->data(index)
                                    .toString()
                                    .toStdString()
                                    .c_str(),
                                allocator);
            }

            entry_obj.AddMember(name, value, allocator);
        }
        entry_array.PushBack(entry_obj, allocator);
    }

    json_save->AddMember("QueryType", _appControl->getQueryType(), allocator);
    json_save->AddMember("Currency", _appControl->getCurrency(), allocator);
    json_save->AddMember("Transactions", entry_array, allocator);
    std::remove(backuppath.c_str());
    if (!_appControl->saveJson(savepath))
    {
        showMsgWindow(QMessageBox::Warning, "Failure",
                      "JSON file cannot be saved");
    }

    // update the piechart
    vector<double> data;
    vector<string> categories;
    _appControl->calcAllocation(categories, data);
}
void AppGui::on_tbtnTransaction_clicked()
{
    ui->tableView->setModel(_transaction_model.get());
    _appControl->stopUpdateTasks();
}

void AppGui::on_tbtnWatchlist_clicked()
{
    if (_transaction_model->rowCount() > 0)
    {
        if (!_watchlist_model)
        {
            initWatchlistModel();
        }

        ui->tableView->setModel(_watchlist_model.get());
        ui->tableView->resizeColumnsToContents();
        _appControl->launchAssetUpdater();
    }
}

void AppGui::closeEvent() { _appControl->stopUpdateTasks(); }

void AppGui::updateWatchlistModel(UpdateData upd_data)
{
    // std::cout << "AppGui::watchlistUpdater get data, id: " << upd_data._id
    //           << endl
    //           << flush;
    QList<QStandardItem *> found_list = _watchlist_model->findItems(
        QString(upd_data._id.c_str()), Qt::MatchContains);

    foreach (QStandardItem *item, found_list)
    { // Update the Grid cells
        QModelIndex index =
            _watchlist_model->index(item->row(), 5, QModelIndex());
        _watchlist_model->setData(
            index, AppControl::floatToString(upd_data._curr_price, 2).c_str());
        index = _watchlist_model->index(item->row(), 6, QModelIndex());
        _watchlist_model->setData(
            index, AppControl::floatToString(upd_data._curr_value, 2).c_str());
        index = _watchlist_model->index(item->row(), 7, QModelIndex());
        _watchlist_model->setData(
            index, AppControl::floatToString(upd_data._diff, 2).c_str());
        int threshold = 10;
        if (upd_data._diff > threshold)
        {
            _watchlist_model->item(item->row(), 7)
                ->setForeground(QBrush("green"));
        }
        else if (upd_data._diff < (-1 * threshold))
        {
            _watchlist_model->item(item->row(), 7)
                ->setForeground(QBrush("red"));
        }
        index = _watchlist_model->index(item->row(), 8, QModelIndex());
        _watchlist_model->setData(
            index,
            (AppControl::floatToString(upd_data._diff_in_percent, 2) + "%")
                .c_str());
        if (upd_data._diff_in_percent > 1)
        {
            _watchlist_model->item(item->row(), 8)
                ->setForeground(QBrush("green"));
        }

        else if (upd_data._diff_in_percent < -1)
        {
            _watchlist_model->item(item->row(), 8)
                ->setForeground(QBrush("red"));
        }
        index = _watchlist_model->index(item->row(), 9, QModelIndex());
        _watchlist_model->setData(
            index, AppControl::floatToString(upd_data._return, 2).c_str());
        if (upd_data._return > threshold)
        {
            _watchlist_model->item(item->row(), 9)
                ->setForeground(QBrush("green"));
        }
        else if (upd_data._return < -threshold)
        {
            _watchlist_model->item(item->row(), 9)
                ->setForeground(QBrush("red"));
        }
        index = _watchlist_model->index(item->row(), 10, QModelIndex());
        _watchlist_model->setData(
            index,
            (AppControl::floatToString(upd_data._return_in_percent, 2) + "%")
                .c_str());
        if (upd_data._return_in_percent > 1)
        {
            _watchlist_model->item(item->row(), 10)
                ->setForeground(QBrush("green"));
        }
        else if (upd_data._return_in_percent < -1)
        {
            _watchlist_model->item(item->row(), 10)
                ->setForeground(QBrush("red"));
        }
    }
    // else
    // {
    //     cout << "AppGui::Warning, ID " << upd_data._id
    //          << " not found on the watchlist!" << endl
    //          << flush;
    // }
    vector<double> data;
    vector<string> categories;
    _appControl->calcCurrentAllocation(categories, data);
    ui->tableView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);
}

// ---------------------------------------------------
// Helper functions section
void AppGui::initTvTransactions(unsigned int row, unsigned int col)
{
    if (_transaction_model)
        _transaction_model->clear();

    _transaction_model = make_shared<QStandardItemModel>(row, col, this);

    // Attach the _transaction_model to the view

    ui->tableView->setModel(_transaction_model.get());
    // editable after key press and double click
    ui->tableView->setEditTriggers(QAbstractItemView::AnyKeyPressed | QAbstractItemView::DoubleClicked);

    for (unsigned int i = 0; i < Config::TRANSACTION_COL_NAMES.size(); i++)
    {
        _transaction_model->setHeaderData(
            i, Qt::Horizontal,
            QObject::tr(Config::TRANSACTION_COL_NAMES[i].c_str()));
    }
}
void AppGui::initWatchlistModel()
{
    shared_ptr<map<string, shared_ptr<Asset>>> const &assets =
        _appControl->getAssets();

    _watchlist_model = make_shared<QStandardItemModel>(
        0, Config::WATCHLIST_COL_NAMES.size(), this);

    // Fill the column labels
    for (unsigned int i = 0; i < Config::WATCHLIST_COL_NAMES.size(); i++)
    {
        _watchlist_model->setHeaderData(
            i, Qt::Horizontal,
            QObject::tr(Config::WATCHLIST_COL_NAMES[i].c_str()));
    }

    int rowPos = 0;
    for (auto it = _appControl->getAssets()->begin();
         it != _appControl->getAssets()->end(); it++)
    {
        QList<QStandardItem *> item_list;
        // item_list<<new QStandardItem(QString("test1"))<<new
        // QStandardItem(QString("test2"));
        item_list << new QStandardItem(QString(it->first.c_str()));
        item_list << new QStandardItem(QString(it->second->getName().c_str()));
        item_list << new QStandardItem(QString(
            AppControl::floatToString(it->second->getAmount(), 2).c_str()));
        item_list << new QStandardItem(QString(
            AppControl::floatToString(it->second->getBalance(), 2).c_str()));
        item_list << new QStandardItem(QString(
            AppControl::floatToString(it->second->getAvgPrice(), 2).c_str()));
        item_list << new QStandardItem(QString(
            AppControl::floatToString(it->second->getCurrPrice(), 2).c_str()));
        item_list << new QStandardItem(QString(
            AppControl::floatToString(it->second->getCurrValue(), 2).c_str()));
        item_list << new QStandardItem(QString(
            AppControl::floatToString(it->second->getDiff(), 2).c_str()));
        item_list << new QStandardItem(
            QString(AppControl::floatToString(it->second->getDiffInPercent(), 2)
                        .c_str()));
        item_list << new QStandardItem(QString(
            AppControl::floatToString(it->second->getReturn(), 2).c_str()));
        item_list << new QStandardItem(QString(
            AppControl::floatToString(it->second->getReturnInPercent(), 2)
                .c_str()));

        _watchlist_model->appendRow(item_list);

        rowPos++;
    }

    ui->tableView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);

    // Starting the updater thread
    _updater.setAppControl(_appControl);
    qRegisterMetaType<UpdateData>("UpdateData");
    connect(&_updater, &UpdaterThread::updatedAsset, this,
            &AppGui::updateWatchlistModel);

    _updater.start();
}

void AppGui::showMsgWindow(QMessageBox::Icon &&msgtype, const std::string title,
                           const std::string msg)
{
    QMessageBox msgWarning;
    QString qmsg = msg.c_str();
    msgWarning.setText(qmsg);
    msgWarning.setIcon(msgtype);
    msgWarning.setWindowTitle(title.c_str());
    msgWarning.exec();
}

void AppGui::createPieChart(vector<string> &categories, vector<double> &data)
{
    _pieseries->clear();
    for (unsigned int i = 0; i < categories.size(); i++)
    {
        _pieseries->append(categories[i].c_str(), data[i]);
    }

    if (_qchart->series().size() == 0)
    {
        _pieseries->setHoleSize(0.35);
        _pieseries->setLabelsVisible();
        // _pieseries->setLabelsPosition(QtCharts::QPieSlice::LabelInsideHorizontal);
        for (auto slice : _pieseries->slices())
            slice->setLabel(slice->label().append(" ").append(
                QString("%1%").arg(100 * slice->percentage(), 0, 'f', 1)));
        _qchart->addSeries(_pieseries);
    }
}
// -------------------------------------------
void UpdaterThread::run()
{
    // std::cout << "UpdaterThread: Start a thread: " << this_thread::get_id()
    //           << endl
    //           << flush;
    while (true)
    {
        // blocking wait call
        if (_is_start)
        {
            unique_ptr<UpdateData> data = _app_control->waitForUpdate();
            UpdateData _update_data(data->_id, data->_curr_price,
                                    data->_curr_value, data->_diff,
                                    data->_diff_in_percent, data->_return,
                                    data->_return_in_percent);

            // std::cout << "UpdaterThread:: incoming data " << _update_data._id
            //           << endl
            //           << flush;
            if (_update_data._id == "disconnect")
            {
                // std::cout << "UpdaterThread::disconnect! " << endl << flush;
                _is_start = false;
                return; // stop the thread
            }

            // send the update data to the main GUI thread. SetPaylod doesn't
            // support unique_ptr
            emit updatedAsset(_update_data);
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }
}
