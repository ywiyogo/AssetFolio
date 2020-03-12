#ifndef GUI_H_
#define GUI_H_

#include "AppControl.h"
#include "MainFrame.h"
#include "MsgQueue.h"
#include "wx/grid.h"
#include "wx/notebook.h"
#include "wx/wx.h"
#include <future>
#include <memory>
#include <wx/dataview.h>
using namespace std;

// main frame containing all control elements
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
    // The Path to the file we have open
    wxString CurrentDocPath;
    wxTextCtrl* MainEditBox;

    wxGrid* _gridActivities;
    wxGrid* _gridWatchlist;
    wxDECLARE_EVENT_TABLE();
    mutex _mutex_ui;
    shared_ptr<AppControl> _appControl;
    future<void> _ftr_updater;
    vector<string> _def_activity_column;
};

class DataGrid : public wxGrid
{
  public:
    DataGrid(wxNotebook* parent);
};

#endif