#ifndef APPGUI_H_
#define APPGUI_H_

#include "AppControl.h"
#include "MainFrame.h"
#include "MsgQueue.h"
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
    wxFrame* m_parent;
    shared_ptr<AppControl> appControl;

  public:
    UpdaterThread(wxFrame* parent, shared_ptr<AppControl> appCtrl)
    {
        m_parent = parent;
        appControl = appCtrl;
    }
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
    string convertFloatToString(float number, int precision);
    void createGridActivities(uint row, uint col);
    void watchlistUpdater();
    void updateWatchlist(wxThreadEvent& event);
    // The Path to the file we have open
    wxString CurrentDocPath;
    wxTextCtrl* MainEditBox;

    wxGrid* _gridActivities;
    wxGrid* _gridWatchlist;

    wxDECLARE_EVENT_TABLE();
    mutex _mutex_ui;
    shared_ptr<AppControl> _appControl;
    // future<void> _ftr_updater;
    unique_ptr<UpdaterThread> updater;
    vector<string> _def_activity_column;

};

#endif