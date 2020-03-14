#ifndef CHARTGUI_H_
#define CHARTGUI_H_

#include <wx/pie/pieplot.h>
#include <wx/category/categorysimpledataset.h>
#include "wx/chart.h"
#include <string>

using namespace std;

class PieChart
{
  public:
    PieChart(string name);
    ~PieChart();
    const wxString& GetName() const;
    Chart* Create(vector<double>& data, vector<string>& categories);
    void Update(double* data, wxString* categories, uint size);
    static wxColour _colours[];
  private:
    wxString _name;
    vector<wxString> _categories;
    CategorySimpleDataset* _dataset ;
    PiePlot* _plot;
    ColorScheme* _colorScheme;
};

#endif