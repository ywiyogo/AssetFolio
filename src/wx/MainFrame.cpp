///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.9.0 Feb 28 2020)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "MainFrame.h"

///////////////////////////////////////////////////////////////////////////

MainFrame::MainFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	_statusBar = this->CreateStatusBar( 1, wxSTB_SIZEGRIP, wxID_ANY );
	m_toolBar1 = this->CreateToolBar( wxTB_HORIZONTAL, wxID_ANY );
	_toolbNew = m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../resource/papirus_new32.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("New"), wxEmptyString, NULL );

	m_toolBar1->AddSeparator();

	_toolbOpen = m_toolBar1->AddTool( wxID_ANY, wxT("open"), wxBitmap( wxT("../resource/papirus_open32.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Open Data"), wxEmptyString, NULL );

	m_toolBar1->AddSeparator();

	_toobSave = m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../resource/papirus_save32.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Save Entries"), wxEmptyString, NULL );

	m_toolBar1->AddSeparator();

	_toolbRefresh = m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../resource/papirus_refresh32.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Refresh/Resize View"), wxEmptyString, NULL );

	m_toolBar1->AddSeparator();

	_toolbInfo = m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../resource/papirus_info32.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("App Info"), wxEmptyString, NULL );

	m_toolBar1->AddSeparator();

	toolbExit = m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../resource/papirus_exit32.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_toolBar1->Realize();

	_bSizerHorizon = new wxBoxSizer( wxHORIZONTAL );

	_bSizerLeft = new wxBoxSizer( wxVERTICAL );

	_btActivities = new wxButton( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );

	_btActivities->SetBitmap( wxBitmap( wxT("../resource/papirus_transaction64.png"), wxBITMAP_TYPE_ANY ) );
	_btActivities->SetToolTip( wxT("Activity List") );

	_bSizerLeft->Add( _btActivities, 0, wxALL|wxEXPAND, 5 );

	_btWatchlist = new wxButton( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );

	_btWatchlist->SetBitmap( wxBitmap( wxT("../resource/papirus_tracker64.png"), wxBITMAP_TYPE_ANY ) );
	_btWatchlist->SetToolTip( wxT("Watchlist and Tracker") );

	_bSizerLeft->Add( _btWatchlist, 0, wxALL|wxEXPAND, 5 );


	_bSizerHorizon->Add( _bSizerLeft, 1, wxALL|wxEXPAND, 5 );

	_bSizerRight = new wxBoxSizer( wxHORIZONTAL );

	_bSizerPanelLeft = new wxBoxSizer( wxVERTICAL );

	_panelLeftActivity = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	_bSizerPanelLeft->Add( _panelLeftActivity, 1, wxEXPAND | wxALL, 5 );


	_bSizerRight->Add( _bSizerPanelLeft, 2, wxEXPAND, 5 );

	_bSizerPanelRight = new wxBoxSizer( wxVERTICAL );


	_bSizerRight->Add( _bSizerPanelRight, 1, wxEXPAND, 5 );


	_bSizerHorizon->Add( _bSizerRight, 10, wxEXPAND, 5 );


	this->SetSizer( _bSizerHorizon );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MainFrame::OnCloseFrame ) );
	this->Connect( _toolbNew->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolNewClicked ) );
	this->Connect( _toolbOpen->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolOpenClicked ) );
	this->Connect( _toobSave->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolSaveClicked ) );
	this->Connect( _toolbRefresh->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolRefreshClicked ) );
	this->Connect( _toolbInfo->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolInfoClicked ) );
	this->Connect( toolbExit->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolExitClicked ) );
	_btActivities->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::onBtnActivitiesClick ), NULL, this );
	_btWatchlist->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::onBtnWatchlistClick ), NULL, this );
}

MainFrame::~MainFrame()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MainFrame::OnCloseFrame ) );
	this->Disconnect( _toolbNew->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolNewClicked ) );
	this->Disconnect( _toolbOpen->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolOpenClicked ) );
	this->Disconnect( _toobSave->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolSaveClicked ) );
	this->Disconnect( _toolbRefresh->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolRefreshClicked ) );
	this->Disconnect( _toolbInfo->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolInfoClicked ) );
	this->Disconnect( toolbExit->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolExitClicked ) );
	_btActivities->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::onBtnActivitiesClick ), NULL, this );
	_btWatchlist->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::onBtnWatchlistClick ), NULL, this );

}
