///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.9.0 Feb 28 2020)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/statusbr.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/toolbar.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class MainFrame
///////////////////////////////////////////////////////////////////////////////
class MainFrame : public wxFrame
{
	private:

	protected:
		wxStatusBar* _statusBar;
		wxToolBar* m_toolBar1;
		wxToolBarToolBase* _toolbNew;
		wxToolBarToolBase* _toolbOpen;
		wxToolBarToolBase* _toobSave;
		wxToolBarToolBase* _toolbRefresh;
		wxToolBarToolBase* _toolbInfo;
		wxToolBarToolBase* toolbExit;
		wxBoxSizer* _bSizerHorizon;
		wxBoxSizer* _bSizerLeft;
		wxButton* _btActivities;
		wxButton* _btWatchlist;
		wxBoxSizer* _bSizerRight;
		wxBoxSizer* _bSizerPanelLeft;
		wxPanel* _panelLeftActivity;
		wxBoxSizer* _bSizerPanelRight;

		// Virtual event handlers, overide them in your derived class
		virtual void OnCloseFrame( wxCloseEvent& event ) = 0;
		virtual void OnToolNewClicked( wxCommandEvent& event ) = 0;
		virtual void OnToolOpenClicked( wxCommandEvent& event ) = 0;
		virtual void OnToolSaveClicked( wxCommandEvent& event ) = 0;
		virtual void OnToolRefreshClicked( wxCommandEvent& event ) = 0;
		virtual void OnToolInfoClicked( wxCommandEvent& event ) = 0;
		virtual void OnToolExitClicked( wxCommandEvent& event ) = 0;
		virtual void onBtnActivitiesClick( wxCommandEvent& event ) = 0;
		virtual void onBtnWatchlistClick( wxCommandEvent& event ) = 0;


	public:

		MainFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Assetfolio - Your Portfolio Tracker That Keeps Assets Private"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1400,900 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~MainFrame();

};

