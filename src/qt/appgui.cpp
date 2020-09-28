#include "appgui.h"
#include "../Config.h"
#include "ui_appgui.h"
#include <QMetaType>
#include <QClipboard>
#include <QDate>
#include <QDateTime>
#include <regex>
#include "customtableview.h"

AppGui::AppGui(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::AppGui),
      _qchart(new QtCharts::QChart()),
      _pieseries(new QtCharts::QPieSeries()),
      _chartView(new QtCharts::QChartView(_qchart)),
      _roiChartView(new CustomGraphicView("Realized RoI & Dividends", QString(Config::DATE_FORMAT.c_str()))),
      _roi_date_series(new QtCharts::QLineSeries()),
      _dividends_date_series(new QtCharts::QLineSeries()),
      _appControl(make_shared<AppControl>(Config::UPDATE_PERIODE)),
      _transaction_model(make_shared<QStandardItemModel>(
          0, Config::TRANSACTION_COL_NAMES.size(), this)),
      _updater()
{
    ui->setupUi(this);
    // init pie chart
    _qchart->setAnimationOptions(QtCharts::QChart::AllAnimations);
    _qchart->legend()->setAlignment(Qt::AlignRight);
    _chartView->setRenderHint(QPainter::Antialiasing);
    _chartView->chart()->setTheme(QtCharts::QChart::ChartThemeDark);
    _chartView->setInteractive(true);
    layout.addWidget(_chartView);
    // set the chart to the tab widget
    ui->tab_alloc->setLayout(&layout);

    layout_roi.addWidget(_roiChartView);
    ui->tab_plots->setLayout(&layout_roi);
}

AppGui::~AppGui() { delete ui; }

void AppGui::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open a JSON file",
                                                    ".", "JSON (*.json);; All Files (*.*)");
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
        isValid = _appControl->readLocalRapidJson(filename.toStdString().c_str());
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

        // check of date format, thanks to https://stackoverflow.com/a/15491967/5700318
        const std::regex re_eudate("(0[1-9]|[12][0-9]|3[01])[- /.](0[1-9]|1[012])[- /.](19|20\\d\\d)$");
        const std::regex re_usdate("(0[1-9]|1[012])[- /.](0[1-9]|[12][0-9]|3[01])[- /.](19|20\\d\\d)$");
        std::smatch submatches;
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
                    string valstring = itr2->value.GetString();
                    if (regex_match(valstring, submatches, re_eudate))
                    {
                        QDate date(stoi(submatches[3].str()), stoi(submatches[2].str()), stoi(submatches[1].str()));
                        _transaction_model->setData(index, date);
                    }
                    else if (regex_match(valstring, submatches, re_usdate))
                    {
                        for (size_t i = 0; i < submatches.size(); ++i)
                        {
                            cout << submatches[i].str() << " ";
                        }
                        cout << endl;
                        QDate date(stoi(submatches[3].str()), stoi(submatches[1].str()), stoi(submatches[2].str()));
                        _transaction_model->setData(index, date);
                    }
                    else
                    {
                        _transaction_model->setData(
                            index, QString(itr2->value.GetString()));
                    }
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

        // Set the DateDelegate for column sorting by date
        ui->tableView->setItemDelegateForColumn(0, new DateDelegate);

        string status_bar = "Total invested value is " + AppControl::floatToString(_appControl->getTotalInvestedValues(), 2) + " " + _appControl->getCurrency().GetString();
        statusBar()->showMessage(tr(status_bar.c_str()));
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
        createRoiChart();
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
    initTvTransactions(10, Config::TRANSACTION_COL_NAMES.size());
}

void AppGui::on_actionExit_triggered()
{
    _appControl->stopUpdateTasks();
    QCoreApplication::quit();
}

void AppGui::on_actionInfo_triggered()
{
    showMsgWindow(
        QMessageBox::Information, "About Assetfolio 1.5",
        "An Asset Portfolio Tracker Application that keeps your asset data "
        "private. We don't need to signup and give up our data to the cloud "
        "server.\nCheck and read the README in "
        "https://github.com/ywiyogo/Assetfolio\n\nDeveloped by Yongkie Wiyogo, "
        "2020");
}

void AppGui::on_actionAddKey_triggered()
{
    bool ok;

    if (!_appControl->readApiKey())
    {
        QString fmp_key = QInputDialog::getText(0, "Input the API Key",
                                                "Please insert your FMP API Key:\n(If you don't have one, please get it from financialmodelingprep.com or\ncontact the developer to buy one.)", QLineEdit::Normal,
                                                "", &ok);
        if (ok && !fmp_key.isEmpty())
        {
            _appControl->setApiKey(fmp_key.toStdString());
        }
    }
    else
    {
        string msg = "FMP API Key found: " + _appControl->getApiKey();
        showMsgWindow(QMessageBox::Information, "API Key", msg);
    }
}

void AppGui::on_actionSave_triggered()
{
    if (_appControl->isEmpty())
    {
        bool ok;
        QStringList items;
        items << tr("EUR") << tr("USD") << tr("RMB") << tr("GBP") << tr("SEK") << tr("CHF") << tr("IDR");
        QString item = QInputDialog::getItem(this, tr("Choose your local currency"),
                                             tr("Currency:"), items, 0, false, &ok);
        if (ok && !items.isEmpty())
            _appControl->setCurrency(item.toStdString());
    }

    QString filename = QFileDialog::getSaveFileName(
        this, "Save transactions to JSON file", ".", "JSON (*.json)");
    if (filename.isNull())
    {
        return;
    }
    try
    {
        shared_ptr<rapidjson::Document> json_save = _appControl->getJsonDoc();
        string savepath = filename.toStdString();
        string backuppath = savepath + ".bak";
        // save a backup
        _appControl->saveJson(backuppath);

        if (json_save->IsObject())
            json_save->RemoveAllMembers();

        json_save->SetObject();
        rapidjson::Document::AllocatorType &allocator = json_save->GetAllocator();
        rapidjson::Value entry_array(rapidjson::kArrayType);

        for (int row = 0; row < _transaction_model->rowCount(); row++)
        {
            rapidjson::Value entry_obj(rapidjson::kObjectType);
            QModelIndex index = _transaction_model->index(row, 0, QModelIndex());

            if (_transaction_model->data(index).toString().toStdString().empty())
            { // first empty row
                continue;
            }

            // string strdate = _transaction_model->data(index).toString().toStdString();
            // int yy;
            // int mm;
            // int dd;
            // sscanf(strdate.c_str(), "%d.%d.%d", &dd, &mm, &yy);
            // cout << "date: " << dd << " month: " << mm << " year: " << yy << endl;
            // if ((dd < 1 || dd > 31) || (mm < 1 || mm > 12) || (yy < 1900 || yy > 2100))
            //     throw runtime_error("Invalid date on row " + std::to_string(row) + " !");

            for (uint col = 0; col < Config::TRANSACTION_COL_NAMES.size(); col++)
            {
                string column_name =
                    _transaction_model->headerData(col, Qt::Horizontal)
                        .toString()
                        .toStdString();
                rapidjson::Value name(column_name.c_str(), allocator);
                rapidjson::Value value;
                index = _transaction_model->index(row, col, QModelIndex());
                string input = _transaction_model->data(index).toString().toStdString();
                if (column_name.compare("Date") == 0)
                {
                    input = _transaction_model->data(index).toDate().toString("dd.MM.yyyy").toStdString();
                    value.SetString(input.c_str(), allocator);
                }
                else if (column_name.compare("ID") == 0)
                {
                    if (input.length() < 2 || input.length() > 12)
                    {
                        throw runtime_error("Invalid ID of row " + std::to_string(row) + " !");
                    }
                    else
                    {
                        value.SetString(input.c_str(), allocator);
                    }
                }
                else if ((column_name.compare("Price") == 0) ||
                         (column_name.compare("Amount") == 0))
                {

                    try
                    {
                        double input_val = stod(input);
                        value.SetDouble(input_val);
                    }
                    catch (const std::invalid_argument &ia)
                    {
                        //when one non numeric value is found, throw exeption
                        throw runtime_error("Invalid value on column " + column_name + " row " + std::to_string(row) + " !");
                    }
                }
                else if (column_name.compare("AssetType") == 0)
                {
                    if (_appControl->isAssetTypeValid(input))
                        value.SetString(input.c_str(), allocator);
                    else
                    {
                        throw runtime_error("Invalid value on Column 'AssetType', row " + std::to_string(row) + " !\nChoose one of these options:\nStock, ETF, Bond, Real Estate, Crypto, Commodity, or Others");
                    }
                }
                else
                {
                    value.SetString(input.c_str(), allocator);
                }
                entry_obj.AddMember(name, value, allocator);
            }
            entry_array.PushBack(entry_obj, allocator);
        }
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
    catch (const std::exception &e)
    {
        showMsgWindow(QMessageBox::Critical, "Failure", e.what());
    }
}
void AppGui::on_tbtnTransaction_clicked()
{
    ui->tableView->setModel(_transaction_model.get());
    ui->tableView->resizeColumnsToContents();
    _appControl->stopUpdateTasks();
    string status_bar = "Total invested value is " + AppControl::floatToString(_appControl->getTotalInvestedValues(), 2) + " " + _appControl->getCurrency().GetString();
    statusBar()->showMessage(tr(status_bar.c_str()));
}

void AppGui::on_tbtnWatchlist_clicked()
{
    if (_transaction_model->rowCount() > 0)
    {
        if (!_watchlist_model)
        {
            on_actionAddKey_triggered();
            initWatchlistModel();
        }

        ui->tableView->setModel(_watchlist_model.get());
        ui->tableView->resizeColumnsToContents();
        ui->tableView->resizeColumnToContents(1);
        try
        {
            _appControl->launchAssetUpdater();
        }
        catch (const std::exception &e)
        {
            showMsgWindow(QMessageBox::Critical, "Failure", e.what());
        }
    }
}

void AppGui::closeEvent(QCloseEvent *event)
{
    _appControl->stopUpdateTasks();
    (void)event; //do nothing, only for inhibit the warning unused parameter
}

void AppGui::updateWatchlistModel(UpdateData upd_data)
{
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

        setWatchlistColor(upd_data._diff, item->row(), 7);
        index = _watchlist_model->index(item->row(), 8, QModelIndex());
        _watchlist_model->setData(
            index,
            (AppControl::floatToString(upd_data._diff_in_percent, 2) + "%")
                .c_str());
        setWatchlistColor(upd_data._diff_in_percent, item->row(), 8, 1);

        index = _watchlist_model->index(item->row(), 9, QModelIndex());
        _watchlist_model->setData(
            index, AppControl::floatToString(upd_data._return, 2).c_str());

        setWatchlistColor(upd_data._return, item->row(), 9);

        index = _watchlist_model->index(item->row(), 10, QModelIndex());
        _watchlist_model->setData(
            index,
            (AppControl::floatToString(upd_data._return_in_percent, 2) + "%")
                .c_str());

        setWatchlistColor(upd_data._return_in_percent, item->row(), 10, 1);

        index = _watchlist_model->index(item->row(), 11, QModelIndex());
        _watchlist_model->setData(
            index,
            (AppControl::floatToString(upd_data._profit_loss, 2)).c_str());
        setWatchlistColor(upd_data._profit_loss, item->row(), 11);
    }

    vector<double> data;
    vector<string> categories;
    _appControl->calcCurrentAllocation(categories, data);
    string status_bar = "Total current asset value is " + AppControl::floatToString(_appControl->getTotalCurrentValues(), 2) + " " + _appControl->getCurrency().GetString();
    statusBar()->showMessage(tr(status_bar.c_str()));
}

// ---------------------------------------------------
// Helper functions section
void AppGui::initTvTransactions(unsigned int row, unsigned int col)
{
    if (_transaction_model)
    {
        _transaction_model->clear();
        if (_watchlist_model)
        {
            _watchlist_model.reset();
        }
    }

    _transaction_model = make_shared<QStandardItemModel>(row, col, this);

    // Attach the _transaction_model to the view

    ui->tableView->setModel(_transaction_model.get());
    // editable after key press and double click
    ui->tableView->setEditTriggers(QAbstractItemView::AnyKeyPressed | QAbstractItemView::DoubleClicked);
    // activate the column sorting
    ui->tableView->setSortingEnabled(true);

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

        item_list << new QStandardItem(QString(
            AppControl::floatToString(it->second->getProfitLoss(), 2)
                .c_str()));

        _watchlist_model->appendRow(item_list);

        rowPos++;
    }

    // ui->tableView->horizontalHeader()->setSectionResizeMode(
    //     QHeaderView::Stretch);

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

void AppGui::createRoiChart()
{
    //prepare the data
    _roi_date_series->clear();
    _dividends_date_series->clear();
    QDateTime mindatetime, maxdatetime;
    QDate initdate(1900, 1, 1);
    mindatetime.setDate(initdate);
    vector<double> values;
    int numTick = 0;
    const map<time_t, float> &totalRoi = _appControl->getTotalRealizedRoi();
    map<QDate, float> orderedROI;
    for (auto it = totalRoi.begin(); it != totalRoi.end(); ++it)
    {
        struct tm *tmp = gmtime(&it->first);
        string date = to_string(tmp->tm_mday) + "." + to_string(tmp->tm_mon + 1) + "." + to_string(tmp->tm_year + 1900);
        QString str_date = date.c_str();
        QStringList date_components = str_date.split(QLatin1Char('.'), Qt::SkipEmptyParts);
        QDate roidate(date_components[2].toInt(), date_components[1].toInt(), date_components[0].toInt());
        if (initdate == mindatetime.date())
        {
            mindatetime.setDate(roidate);
        }
        else if (roidate < mindatetime.date())
        {
            if (mindatetime.date() > maxdatetime.date())
                maxdatetime.setDate(mindatetime.date());
            mindatetime.setDate(roidate);
        }
        else if (maxdatetime.date() < roidate)
        {
            maxdatetime.setDate(roidate);
        }
        numTick++;
        
        orderedROI.emplace(roidate, it->second);
        values.push_back(it->second);
    }
    for (auto iter = orderedROI.begin(); iter != orderedROI.end(); ++iter)
    {

        _roi_date_series->append(QDateTime(iter->first).toMSecsSinceEpoch(), iter->second);
    }
    const map<time_t, float> &totalDividends = _appControl->getAccDividends();

    for (auto iter = totalDividends.begin(); iter != totalDividends.end(); ++iter)
    {

        struct tm *tmp = gmtime(&iter->first);
        string date = to_string(tmp->tm_mday) + "." + to_string(tmp->tm_mon + 1) + "." + to_string(tmp->tm_year + 1900);
        QString str_date = date.c_str();
        QStringList date_components = str_date.split(QLatin1Char('.'), Qt::SkipEmptyParts);
        QDate roidate(date_components[2].toInt(), date_components[1].toInt(), date_components[0].toInt());
        _dividends_date_series->append(QDateTime(roidate).toMSecsSinceEpoch(), iter->second);
    }
    
    // Set the X-Y axes and the data series to the chart
    // Attach axes after adding the data series to the chart
    if (_roi_date_series->attachedAxes().size() == 0)
    {
        if (numTick < 5)
            numTick = numTick * 2;
        else if (numTick > 12)
            numTick = 12;

        _roiChartView->connectDataSeries(_roi_date_series, numTick);
        _roiChartView->connectDataSeries(_dividends_date_series, numTick);
    }
    
    // Update the axis ranges
    _roiChartView->axisX()->setRange(mindatetime, maxdatetime);
    if (values.size() > 1)
    {
        std::vector<double>::iterator minIterator = min_element(values.begin(), values.end());
        std::vector<double>::iterator maxIterator = max_element(values.begin(), values.end());
        _roiChartView->axisY()->setRange(*minIterator, *maxIterator);
    }
    else
        _roiChartView->axisY()->setRange(0, 1);

    QString title = "RoI in ";
    title += _appControl->getCurrency().GetString();
    _roiChartView->axisY()->setTitleText(title);
    _roiChartView->setRenderHint(QPainter::Antialiasing);
}

void AppGui::setWatchlistColor(float update_var, uint rowidx, uint colidx, float threshold)
{
    if (update_var > threshold)
    {
        _watchlist_model->item(rowidx, colidx)
            ->setForeground(QBrush("green"));
    }
    else if (update_var < (-1 * threshold))
    {
        _watchlist_model->item(rowidx, colidx)
            ->setForeground(QBrush("red"));
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
                                    data->_return_in_percent,
                                    data->_profit_loss);

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
