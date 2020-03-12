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
	_bSizerHorizon = new wxBoxSizer( wxHORIZONTAL );

	_bSizerLeft = new wxBoxSizer( wxVERTICAL );

	_btActivities = new wxButton( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );

	_btActivities->SetBitmap( wxBitmap( wxT("../icons/papirus_transaction64.png"), wxBITMAP_TYPE_ANY ) );
	_btActivities->SetToolTip( wxT("Activity List") );

	_bSizerLeft->Add( _btActivities, 0, wxALL|wxEXPAND, 5 );

	_btWatchlist = new wxButton( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );

	_btWatchlist->SetBitmap( wxBitmap( wxT("../icons/papirus_tracker64.png"), wxBITMAP_TYPE_ANY ) );
	_btWatchlist->SetToolTip( wxT("Watchlist") );

	_bSizerLeft->Add( _btWatchlist, 0, wxALL|wxEXPAND, 5 );

	_btCharts = new wxButton( this, wxID_ANY, wxT("Charts"), wxDefaultPosition, wxDefaultSize, 0 );
	_btCharts->Enable( false );
	_btCharts->Hide();

	_bSizerLeft->Add( _btCharts, 0, wxALL|wxEXPAND, 5 );


	_bSizerHorizon->Add( _bSizerLeft, 1, wxALL|wxEXPAND, 5 );

	_bSizerRight = new wxBoxSizer( wxVERTICAL );

	_sbSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxEmptyString ), wxVERTICAL );


	_bSizerRight->Add( _sbSizer, 1, wxEXPAND, 5 );


	_bSizerHorizon->Add( _bSizerRight, 10, wxEXPAND, 5 );


	this->SetSizer( _bSizerHorizon );
	this->Layout();
	m_toolBar1 = this->CreateToolBar( wxTB_HORIZONTAL, wxID_ANY );
	_toolbNew = m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../icons/papirus_new32.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("New"), wxEmptyString, NULL );

	m_toolBar1->AddSeparator();

	_toolbOpen = m_toolBar1->AddTool( wxID_ANY, wxT("open"), wxBitmap( wxT("../icons/papirus_open32.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Open Data"), wxEmptyString, NULL );

	m_toolBar1->AddSeparator();

	_toobSave = m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../icons/papirus_save32.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Save Entries"), wxEmptyString, NULL );

	m_toolBar1->AddSeparator();

	_toolbKey = m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../icons/papirus_key32.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_toolBar1->AddSeparator();

	_toolbRefresh = m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../icons/papirus_refresh32.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Refresh/Resize View"), wxEmptyString, NULL );

	m_toolBar1->AddSeparator();

	_toolbInfo = m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../icons/papirus_info32.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("App Info"), wxEmptyString, NULL );

	m_toolBar1->AddSeparator();

	_toolbExit = m_toolBar1->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../icons/papirus_exit32.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("Exit Application"), wxEmptyString, NULL );

	m_toolBar1->Realize();


	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MainFrame::OnCloseFrame ) );
	_btActivities->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::onBtnActivitiesClick ), NULL, this );
	_btWatchlist->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::onBtnWatchlistClick ), NULL, this );
	this->Connect( _toolbNew->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolNewClicked ) );
	this->Connect( _toolbOpen->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolOpenClicked ) );
	this->Connect( _toobSave->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolSaveClicked ) );
	this->Connect( _toolbKey->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolKeyClicked ) );
	this->Connect( _toolbRefresh->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolRefreshClicked ) );
	this->Connect( _toolbInfo->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolInfoClicked ) );
	this->Connect( _toolbExit->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolExitClicked ) );
}

MainFrame::~MainFrame()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MainFrame::OnCloseFrame ) );
	_btActivities->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::onBtnActivitiesClick ), NULL, this );
	_btWatchlist->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::onBtnWatchlistClick ), NULL, this );
	this->Disconnect( _toolbNew->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolNewClicked ) );
	this->Disconnect( _toolbOpen->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolOpenClicked ) );
	this->Disconnect( _toobSave->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolSaveClicked ) );
	this->Disconnect( _toolbKey->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolKeyClicked ) );
	this->Disconnect( _toolbRefresh->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolRefreshClicked ) );
	this->Disconnect( _toolbInfo->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolInfoClicked ) );
	this->Disconnect( _toolbExit->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnToolExitClicked ) );

}
