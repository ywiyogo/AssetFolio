// Author: YWiyogo
// Descr.: The application GUI

#include "AppGui.h"
#include "AppControl.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "wx/grid.h"
#include "wx/log.h"
#include <cstdio>
#include <iomanip> // std::setprecision
#include <sstream> // stringstream
#include <string>
#include <vector>
using namespace std;

// size of chatbot window
const int WIN_WIDTH = 1000;
const int WIN_HEIGHT = 800;

int AppGui::wxIdCounter = 1;

// The periode of the REST API updater in sec
const uint UPDATE_PERIODE = 20;

// event mapping
wxDEFINE_EVENT(UPDATER_EVENT, wxThreadEvent);
wxBEGIN_EVENT_TABLE(AppGui, wxFrame) wxEND_EVENT_TABLE();

// wxWidgets FRAME

AppGui::AppGui()
    : MainFrame(nullptr), _gridActivities(nullptr), _gridWatchlist(nullptr),
      _appControl(make_shared<AppControl>(UPDATE_PERIODE)), _updater{},
      _chartPanel(nullptr), _pie_chart(nullptr)
{
    _def_activity_column = {"Date", "ID",          "Name",   "AssetType",
                            "Type", "Transaction", "Amount", "Broker"};

    _bSizerHorizon->Layout();
    Bind(UPDATER_EVENT, &AppGui::updateWatchlist, this);
}
AppGui::~AppGui() {}

void AppGui::initWatchlistGrid()
{
    shared_ptr<map<string, shared_ptr<Asset>>> const& assets =
        _appControl->getAssets();
    vector<string> labels{"ID",        "Name",       "Amount",     "Balance",
                          "Avg Price", "Curr.Price", "Curr.Value", "Diff",
                          "Diff %",    "Return",     "Return %"};

    _panelLeftWatchlist = new wxPanel(this, wxID_ANY, wxDefaultPosition,
                                      wxDefaultSize, wxTAB_TRAVERSAL);
    _gridWatchlist = new wxGrid(_panelLeftWatchlist, wxID_ANY,
                                wxDefaultPosition, wxDefaultSize, 0);
    _bSizerPanelLeft->Detach(0);
    _bSizerPanelLeft->Prepend(_panelLeftWatchlist, 1, wxEXPAND | wxALL, 5);
    _bSizerPanelLeft->Fit(_panelLeftWatchlist);
    _bSizerPanelRight->Fit(_chartPanel);
    _panelLeftWatchlist->Show();

    _gridWatchlist->CreateGrid(assets->size(), labels.size());
    _gridWatchlist->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_TOP);
    _gridWatchlist->SetRowLabelSize(30);

    // Fill the column labels
    for (int i = 0; i < labels.size(); i++)
    {
        _gridWatchlist->SetColLabelValue(i, wxString(labels[i]));
    }

    int rowPos = 0;
    for (auto it = _appControl->getAssets()->begin();
         it != _appControl->getAssets()->end(); it++)
    {
        _gridWatchlist->SetCellValue(rowPos, 0, it->first);
        _gridWatchlist->SetCellValue(rowPos, 1, it->second->getName());
        _gridWatchlist->SetCellValue(rowPos, 2,
                                     floatToString(it->second->getAmount(), 2));
        _gridWatchlist->SetCellValue(
            rowPos, 3, floatToString(it->second->getBalance(), 2));
        _gridWatchlist->SetCellValue(
            rowPos, 4, floatToString(it->second->getAvgPrice(), 2));
        _gridWatchlist->SetCellValue(
            rowPos, 5, floatToString(it->second->getCurrPrice(), 2));
        _gridWatchlist->SetCellValue(
            rowPos, 6, floatToString(it->second->getCurrValue(), 2));
        _gridWatchlist->SetCellValue(rowPos, 7,
                                     floatToString(it->second->getDiff(), 2));
        _gridWatchlist->SetCellValue(
            rowPos, 8, floatToString(it->second->getDiffInPercent(), 2));
        _gridWatchlist->SetCellValue(rowPos, 9,
                                     floatToString(it->second->getReturn(), 2));
        _gridWatchlist->SetCellValue(
            rowPos, 10, floatToString(it->second->getReturnInPercent(), 2));

        rowPos++;
    }
    _gridWatchlist->AutoSize();
    // trigger the assets update
    _gridWatchlist->Layout();

    // Starting the updater wxThread. Note, wxWidget GUI doesn't support async.
    // It is recommended to use wxThread in order to avoid UI issues
    _updater = make_unique<UpdaterThread>(this, _appControl);

    if (_updater->Create() != wxTHREAD_NO_ERROR)
    {
        wxMessageBox(_("Couldn't create thread!"));
        return;
    }

    if (_updater->Run() != wxTHREAD_NO_ERROR)
    {
        wxMessageBox(_("Couldn't run thread!"));
        return;
    }
}
void AppGui::OnCloseFrame(wxCloseEvent& event)
{
    if (_gridWatchlist)
    {
        cout << "AppGui::deactivate all threads" << endl << flush;
        // terminate the updater task
        _gridWatchlist->Show(false);

        _appControl->stopUpdateTasks();
    }
    cout << "AppGui::destroy" << endl << flush;
    Destroy();
}

void AppGui::onBtnActivitiesClick(wxCommandEvent& event)
{
    if (_gridActivities)
    {
        if (_panelLeftActivity->IsShown())
        {
            // do nothing
        }
        else
        {
            // _gridActivities->Show(true);
            if (_gridWatchlist)
            {
                _panelLeftWatchlist->Hide();
                _bSizerPanelLeft->Detach(0);
                _bSizerPanelLeft->Prepend(_panelLeftActivity, 2,
                                          wxEXPAND | wxALL, 5);
                _panelLeftActivity->Show();
                // Stop the asset updater send tasks but not the waiting receive
                // task
                _appControl->stopUpdateTasks();
                _bSizerPanelLeft->Layout();
                _bSizerPanelRight->Layout();
                _bSizerHorizon->Layout();
            }
        }
    }
}
void AppGui::onBtnWatchlistClick(wxCommandEvent& event)
{
    if (_gridActivities && !_gridWatchlist)
    {
        _panelLeftActivity->Hide();

        initWatchlistGrid();

        _appControl->launchAssetUpdater();
    }
    else
    {

        if (_gridActivities && !_panelLeftWatchlist->IsShown())
        {
            _panelLeftActivity->Hide();
            _bSizerPanelLeft->Detach(0);
            _bSizerPanelLeft->Prepend(_panelLeftWatchlist, 2, wxEXPAND | wxALL,
                                      5);
            _panelLeftWatchlist->Show();
            _updater->restart();
            _appControl->launchAssetUpdater();
        }
    }
    _bSizerRight->Layout();
    _bSizerHorizon->Layout();
}

void AppGui::onBtnChartsClick(wxCommandEvent& event)
{
    if (_gridActivities)
        _gridActivities->Show(false);
    if (_gridWatchlist)
        _gridWatchlist->Show(false);

    _bSizerHorizon->Layout();
}

//-----------------------------
// Toolbar Events
void AppGui::OnToolNewClicked(wxCommandEvent& event)
{
    _appControl.reset(new AppControl(UPDATE_PERIODE));
    createGridActivities(10, 8);
}

void AppGui::OnToolOpenClicked(wxCommandEvent& event)
{
    // Open the File dialog
    wxFileDialog* OpenDialog = new wxFileDialog(
        this, "Choose a file to open", wxEmptyString, wxEmptyString,
        "JSON (*.jsn, *.json)|*.jsn;*.json", wxFD_OPEN);

    // Creates a "open file" dialog with 4 file types
    // if the user click "Open" instead of "Cancel"
    if (OpenDialog->ShowModal() == wxID_OK)
    {
        CurrentDocPath = OpenDialog->GetPath();
        // Showing the JSON file in the spreadsheet
        bool isValid = false;

        try
        {
            isValid = _appControl->readLocalRapidJson(CurrentDocPath.c_str(),
                                                      _def_activity_column);
        }
        catch (const exception& e)
        {
            wxLogError(e.what());
        }
        // read and check the JSON file
        if (isValid)
        {
            shared_ptr<rapidjson::Document> jsonDoc = _appControl->getJsonDoc();

            auto json_entries = jsonDoc->GetObject()["Activities"].GetArray();

            int rowPos = 0;
            int colPos = 0;
            int offset = 10;

            int numOfRows = json_entries.Size() + offset;
            createGridActivities(numOfRows, json_entries[0].MemberCount());

            for (rapidjson::Value::ConstValueIterator itr =
                     json_entries.Begin();
                 itr != json_entries.End(); ++itr)
            {

                // creating the asset object
                rapidjson::Value::ConstMemberIterator itr2;
                for (itr2 = itr->MemberBegin(); itr2 != itr->MemberEnd();
                     ++itr2)
                {
                    if (rowPos == 0)
                    { // set the column label name
                        _gridActivities->SetColLabelValue(
                            colPos, wxString(itr2->name.GetString()));
                    }
                    if (itr2->value.IsString())
                    {
                        _gridActivities->SetCellValue(
                            rowPos, colPos, wxString(itr2->value.GetString()));
                    }
                    else if (itr2->value.IsNumber())
                    {
                        _gridActivities->SetCellValue(
                            rowPos, colPos,
                            wxString(floatToString(itr2->value.GetFloat(), 2)));
                    }
                    else
                    {
                        cout << "Label: " << itr2->name.GetString() << flush;
                        cout << "Value type is: " << itr2->value.GetType()
                             << endl;
                    }
                    colPos++;
                }
                rowPos++;
                colPos = 0;
            }
            _gridActivities->AutoSize();

            // Fill the watchlist viewer
            vector<string> colWatchlist = {
                "Name",         "Amount",      "Total transaktion",
                "Buying price", "Curr. Price", "Curr. Asset",
                "Allocation",   "Changed %",   "Change",
                "Yield %",      "TotalYield"};

            createPieChart();
            _bSizerHorizon->Layout();
        }
        else
        {
            wxLogError("JSON data is not valid");
        }
    }

    // Clean up after ourselves
    OpenDialog->Destroy();
}

void AppGui::OnToolRefreshClicked(wxCommandEvent& event)
{
    if (_gridActivities && _gridActivities->IsShown())
    {
        _gridActivities->AutoSize();
    }
    if (_gridWatchlist && _gridWatchlist->IsShown())
    {
        _gridWatchlist->AutoSize();
    }
}
void AppGui::OnToolSaveClicked(wxCommandEvent& event)
{
    wxFileDialog* SaveDialog =
        new wxFileDialog(this, _("Save File As _?"), wxEmptyString,
                         wxEmptyString, _("JSON (*.jsn, *.json)|*.jsn;*.json"),
                         wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

    // Creates a Save Dialog with 4 file types
    if (SaveDialog->ShowModal() == wxID_OK) // If the user clicked "OK"
    {
        // set the path of our current document to the file the user chose to
        // save under
        if (_appControl && _gridActivities)
        {
            uint column_size = _gridActivities->GetNumberCols();
            vector<string> col_names;
            shared_ptr<rapidjson::Document> json_save =
                _appControl->getJsonDoc();
            string savepath = SaveDialog->GetPath().ToStdString();
            string backuppath = savepath + ".bak";
            // save a backup
            _appControl->saveJson(backuppath);
            json_save->RemoveAllMembers();
            json_save->SetObject();
            rapidjson::Document::AllocatorType& allocator =
                json_save->GetAllocator();
            for (uint col = 0; col < column_size; col++)
            {
                col_names.push_back(
                    _gridActivities->GetCellValue(0, col).ToStdString());
            }

            rapidjson::Value entry_array(rapidjson::kArrayType);

            for (uint row = 0; row < _gridActivities->GetNumberRows(); row++)
            {
                rapidjson::Value entry_obj(rapidjson::kObjectType);

                if (_gridActivities->GetCellValue(row, 0).ToStdString().empty())
                { // first empty row
                    break;
                }

                for (uint col = 0; col < column_size; col++)
                {

                    string column_name =
                        _gridActivities->GetColLabelValue(col).ToStdString();
                    rapidjson::Value name(column_name.c_str(), allocator);
                    rapidjson::Value value;

                    if ((column_name.compare("Transaction") == 0) ||
                        (column_name.compare("Amount") == 0))
                    {
                        value.SetFloat(stringToFloat(
                            _gridActivities->GetCellValue(row, col)
                                .ToStdString(),
                            2));
                    }
                    else
                    {
                        value.SetString(_gridActivities->GetCellValue(row, col)
                                            .ToStdString()
                                            .c_str(),
                                        allocator);
                    }

                    entry_obj.AddMember(name, value, allocator);
                }
                entry_array.PushBack(entry_obj, allocator);
            }

            json_save->AddMember("Activities", entry_array, allocator);
            remove(backuppath.c_str());
            if (!_appControl->saveJson(savepath))
            {
                wxLogError("Saving JSON file has failed.");
            }

            // update the piechart
            vector<double> data;
            vector<string> categories;
            _appControl->calcAllocation(categories, data);
            _pie_chart->Create(categories, data);
            _bSizerRight->Layout();
        }
    }

    // Clean up after ourselves
    SaveDialog->Destroy();
}
void AppGui::OnToolExitClicked(wxCommandEvent& event) { Close(); }

void AppGui::OnToolInfoClicked(wxCommandEvent& event)
{
    wxMessageBox(
        "An Asset Portfolio Tracker Application that keeps your asset data "
        "private. We don't need to signup and give up our data to the cloud "
        "server.\nCheck and read the README in "
        "https://github.com/ywiyogo/Assetfolio\n\nDeveloped by Yongkie Wiyogo, "
        "2020",
        "About Assetfolio", wxOK | wxICON_INFORMATION);
}

// ---------------------------------------------------
// Helper functions section
void AppGui::createGridActivities(uint row, uint col)
{
    if (_gridActivities)
    {
        delete _gridActivities;
        _gridActivities = nullptr;
        _appControl->clearJsonData();
    }
    _gridActivities = new wxGrid(_panelLeftActivity, wxID_ANY,
                                 wxDefaultPosition, wxDefaultSize, 0);

    _gridActivities->CreateGrid(row, col);
    _gridActivities->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_TOP);
    _gridActivities->EnableEditing(true);
    _gridActivities->EnableGridLines(true);
    _gridActivities->EnableDragGridSize(false);
    _gridActivities->SetMargins(5, 5);
    _gridActivities->SetRowLabelSize(30);
    for (int i = 0; i < _def_activity_column.size(); i++)
    {
        _gridActivities->SetColLabelValue(i, wxString(_def_activity_column[i]));
    }
    _gridActivities->AutoSize();
}

void AppGui::createPieChart()
{
    vector<double> data;
    vector<string> categories;
    _appControl->calcAllocation(categories, data);
    bool is_exist = false;
    if (_chartPanel)
    {
        is_exist = true;
        _bSizerPanelRight->Detach(0);
        wxDELETE(_chartPanel);
    }

    _pie_chart = new PieChart("Asset Allocation");
    _chartPanel = new wxChartPanel(this);

    _chartPanel->SetChart(_pie_chart->Create(categories, data));

    _bSizerPanelRight->Add(_chartPanel, 1, wxEXPAND | wxALL, 5);

    _bSizerPanelRight->Fit(_chartPanel);
    _bSizerRight->Layout();
}
string AppGui::floatToString(float number, int precision)
{
    stringstream stream;
    stream << fixed << setprecision(precision) << number;
    return stream.str();
}
float AppGui::stringToFloat(string numstr, int precision)
{
    stringstream stream;
    stream << fixed << setprecision(precision) << stof(numstr);
    return stof(stream.str());
    ;
}

//-----------------------------
// WxThread functions section
void AppGui::updateWatchlist(wxThreadEvent& event)
{
    // send the update data to the main GUI thread. SetPaylod doesn't support
    // unique_ptr
    UpdateData upd_data = event.GetPayload<UpdateData>();

    cout << "AppGui::watchlistUpdater get data, id: " << upd_data._id << endl
         << flush;
    int found_row = -1;

    for (int i = 0; i < _gridWatchlist->GetRows(); i++)
    { // check if the id exists on the table
        if (_gridWatchlist->GetCellValue(i, 0).ToStdString() == upd_data._id)
        {
            found_row = i;
            break;
        }
    }
    if (found_row > -1)
    { // Update the Grid cells

        _gridWatchlist->SetCellValue(found_row, 5,
                                     floatToString(upd_data._curr_price, 2));
        _gridWatchlist->SetCellValue(found_row, 6,
                                     floatToString(upd_data._curr_value, 2));

        _gridWatchlist->SetCellValue(found_row, 7,
                                     floatToString(upd_data._diff, 2));
        int threshold = 10;
        if (upd_data._diff > threshold)
        {
            _gridWatchlist->SetCellTextColour(found_row, 7, *wxGREEN);
        }
        else if (upd_data._diff < (-1 * threshold))
        {
            _gridWatchlist->SetCellTextColour(found_row, 7, *wxRED);
        }

        _gridWatchlist->SetCellValue(
            found_row, 8, floatToString(upd_data._diff_in_percent, 2) + "%");
        if (upd_data._diff_in_percent > 1)
        {
            _gridWatchlist->SetCellTextColour(found_row, 8, *wxGREEN);
        }
        else if (upd_data._diff_in_percent < -1)
        {
            _gridWatchlist->SetCellTextColour(found_row, 8, *wxRED);
        }

        _gridWatchlist->SetCellValue(found_row, 9,
                                     floatToString(upd_data._return, 2));
        if (upd_data._return > threshold)
        {
            _gridWatchlist->SetCellTextColour(found_row, 9, *wxGREEN);
        }
        else if (upd_data._return < -threshold)
        {
            _gridWatchlist->SetCellTextColour(found_row, 9, *wxRED);
        }

        _gridWatchlist->SetCellValue(
            found_row, 10, floatToString(upd_data._return_in_percent, 2) + "%");
        if (upd_data._return_in_percent > 1)
        {
            _gridWatchlist->SetCellTextColour(found_row, 10, *wxGREEN);
        }
        else if (upd_data._return_in_percent < -1)
        {
            _gridWatchlist->SetCellTextColour(found_row, 10, *wxRED);
        }
    }
    else
    {
        cout << "AppGui::Warning, ID " << upd_data._id
             << " not found on the watchlist!" << endl
             << flush;
    }
    vector<double> data;
    vector<string> categories;
    _appControl->calcCurrentAllocation(categories, data);
    _gridWatchlist->AutoSize();

    _bSizerPanelLeft->Fit(_panelLeftWatchlist);
    _bSizerPanelRight->Fit(_chartPanel);
    _bSizerHorizon->Layout();
}

wxThread::ExitCode UpdaterThread::Entry()
{
    cout << "UpdaterThread: Start a thread: " << this_thread::get_id() << endl
         << flush;

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

            cout << "UpdaterThread:: incoming data " << _update_data._id << endl
                 << flush;
            if (_update_data._id == "disconnect")
            {
                cout << "UpdaterThread::disconnect! " << endl << flush;
                _is_start = false;
            }

            wxThreadEvent event(UPDATER_EVENT);
            // send the update data to the main GUI thread. SetPaylod doesn't
            // support unique_ptr
            event.SetPayload(_update_data);

            _parent->GetEventHandler()->AddPendingEvent(event);
            cout << "UpdaterThread:: sent to main GUI " << endl << flush;
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }
    cout << "UpdaterThread::watchlistUpdater ends" << endl << flush;
    return 0;
}