#include "main.h"
#include "AppGui.h"

// wxWidgets APP
IMPLEMENT_APP(Assetfolio);

std::string dataPath = "../";
std::string imgBasePath = dataPath + "images/";

bool Assetfolio::OnInit()
{
    // create window with name and show it
    AppGui *mainGui = new AppGui();
    mainGui->Show(true);

    return true;
}