// Author: YWiyogo
// Descr.: Implementation of all chart types (pie chart or xy-plotting)

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
    Chart* Create(vector<string>& categories, vector<double>& data);
    void Update(wxString* categories, double* data, uint size);
    static wxColour _colours[];

  private:
    wxString _name;
    // Note: the unique_ptr cannot be used because wxFreeChart doesn't support it yet.
    CategorySimpleDataset* _dataset;
    PiePlot* _plot;
    ColorScheme* _colorScheme;
};

#endif