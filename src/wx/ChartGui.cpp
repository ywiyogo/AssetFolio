// Author: YWiyogo
// Descr.: Implementation of all chart types (pie chart or xy-plotting)

#include "ChartGui.h"
#include <iomanip>
#include <sstream>
const int MAX_CATEGORIES = 16;
wxColour PieChart::_colours[] = {
    wxColour("#B29DD9"), // Light Pastel Purple
    wxColour("#FDFD98"), // Pastel Yellow
    wxColour("#FE6B64"), // Pastel Red
    wxColour("#77DD77"), // Pastel Green
    wxColour("#779ECB"), // Dark Pastel Blue
    wxColour("#FCEEC5"), // Blond
    wxColour("#E0FEFE"), // Light Cyan
    wxColour("#C7CEEA"), // Periwinkle (Crayola)
    wxColour("#FFDAC1"), // Very Pale Orange
    wxColour("#998AD3"), // Ceil
    wxColour("#FF9AA2"), // Light Salmon Pink
    wxColour("#E494D3"), // Orchid
    wxColour("#FFFFD8"), // Light Yellow
    wxColour("#B5EAD7"), // Magic Mint
    wxColour("#CDF1AF"), // Tea Green
    wxColour("#F1C470"), // Mellow Apricot
};
PieChart::PieChart(string name)
    : _name(wxString(name)), _dataset(nullptr), _plot(nullptr),
      _colorScheme(new ColorScheme(_colours, WXSIZEOF(_colours))),
      _chart(nullptr), _cat_renderer(nullptr)
{
}

PieChart::~PieChart()
{
    if (_dataset)
    {
        delete _dataset;
        _dataset = nullptr;
    }

    if (_plot)
    {
        delete _plot;
        _plot = nullptr;
    }

    if (_colorScheme)
    {
        delete _colorScheme;
        _colorScheme = nullptr;
    }
    if (_chart)
    {
        delete _chart;
        _chart = nullptr;
    }
    if (_cat_renderer)
    {
        delete _cat_renderer;
        _cat_renderer = nullptr;
    }
    cout<<"deleted chart"<<endl;
}
const wxString& PieChart::GetName() const { return _name; }

Chart* PieChart::Create(vector<string>& categories, vector<double>& data)
{
    // first step: create plot
    if (!_plot)
        _plot = new PiePlot();

    // create dataset
    wxString pie_categories[categories.size()];
    double max = 0;
    for (int i = 0; i < data.size(); i++)
    {
        max += data[i];
    }
    for (int i = 0; i < categories.size(); i++)
    {
        stringstream stream;
        stream << fixed << setprecision(1) << data[i] / max * 100.;
        string percent = " (" + stream.str() + "%)";
        pie_categories[i] = wxString(categories[i] + percent);
    }
    double pie_data[data.size()];
    copy(data.begin(), data.end(), pie_data);

    if (categories.size() > MAX_CATEGORIES)
    {
        string msg = "Max number of asset " + to_string(MAX_CATEGORIES) +
                     " is reached! Please contact the developer";
        wxLogWarning(msg.c_str());
    }
    Update(pie_categories, pie_data, data.size());
    
    // and finally create chart
    if (!_chart)
    {
        // set legend to plot
        _plot->SetLegend(new Legend(wxBOTTOM, wxCENTER));
        _chart = new Chart(_plot, GetName());
    }

    return _chart;
}

void PieChart::Update(wxString* categories, double* data, uint size)
{
    if (!_dataset)
        _dataset = new CategorySimpleDataset(categories, size);

    // and add serie to it
    _dataset->AddSerie(_("Serie 1"), data, size);

    // create category renderer for legend drawing
    if(!_cat_renderer)
        _cat_renderer = new CategoryRenderer(*_colorScheme);
    _dataset->SetRenderer(_cat_renderer);

    // set color scheme
    _plot->SetColorScheme(_colorScheme);

    _plot->SetDataset(_dataset);
}

void PieChart::Clear()
{
    delete _plot;
}