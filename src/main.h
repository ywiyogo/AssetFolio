#ifndef MAIN_H_
#define MAIN_H_

#include <wx/wx.h>

// wxWidgets app that hides main()
class Assetfolio : public wxApp
{
public:
    // events
    virtual bool OnInit();
};

#endif