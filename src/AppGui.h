// Author: YWiyogo
// Descr.: The application GUI

#ifndef APPGUI_H_
#define APPGUI_H_

#include "AppControl.h"
#include "ChartGui.h"
#include "MainFrame.h"
#include "MsgQueue.h"
#include "wx/chartpanel.h"
#include "wx/grid.h"
#include "wx/wx.h"
#include <memory>

using namespace std;

//-----------------------------------------------
//              Wx Custom Event
//-----------------------------------------------
wxDECLARE_EVENT(UPDATER_EVENT, wxThreadEvent);
// the ID we'll use to identify our event
const int WATCHLIST_UPD_ID = 10000;

//-----------------------------------------------
//              Wx Thread
// a thread class contains a message queue and send events to the GUI thread
//-----------------------------------------------
class UpdaterThread : public wxThread
{
    wxFrame* _parent;
    shared_ptr<AppControl> _app_control;
    bool _is_start;

  public:
    UpdaterThread(wxFrame* parent, shared_ptr<AppControl> appCtrl)
    {
        _parent = parent;
        _app_control = appCtrl;
        _is_start = true;
    }
    void restart() { _is_start = true; }
    // implement the pure virtual function from wxThread
    virtual ExitCode Entry();
};

// ------------------------------------------------
// main frame containing all control elements
// ----------------------------------------------
class AppGui : public MainFrame
{
  public:
    AppGui();
    ~AppGui();

  protected:
    // Virtual event handlers, overide them in your derived class
    void OnCloseFrame(wxCloseEvent& event);
    void onBtnActivitiesClick(wxCommandEvent& event);
    void onBtnWatchlistClick(wxCommandEvent& event);
    void onBtnChartsClick(wxCommandEvent& event);
    void OnToolNewClicked(wxCommandEvent& event);
    void OnToolOpenClicked(wxCommandEvent& event);
    void OnToolKeyClicked(wxCommandEvent& event);
    void OnToolRefreshClicked(wxCommandEvent& event);
    void OnToolSaveClicked(wxCommandEvent& event);
    void OnToolExitClicked(wxCommandEvent& event);
    void OnToolInfoClicked(wxCommandEvent& event);

  private:
    static int wxIdCounter;
    void initWatchlistGrid();
    string floatToString(float number, int precision);
    float stringToFloat(string numstr, int precision);
    void createGridActivities(uint row, uint col);
    void createPieChart();
    void watchlistUpdater();
    void updateWatchlist(wxThreadEvent& event);

    // The Path to the file we have open
    wxString CurrentDocPath;
    wxTextCtrl* MainEditBox;
    wxGrid* _gridActivities;
    wxGrid* _gridWatchlist;
    wxPanel* _panelLeftWatchlist;
    wxDECLARE_EVENT_TABLE();
    mutex _mutex_ui;
    shared_ptr<AppControl> _appControl;
    // future<void> _ftr_updater;
    unique_ptr<UpdaterThread> _updater;
    vector<string> _def_activity_column;

    wxChartPanel* _chartPanel;
    PieChart* _pie_chart;
};

#endif