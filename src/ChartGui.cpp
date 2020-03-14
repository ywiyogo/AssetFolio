#include "ChartGui.h"

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
      _colorScheme(new ColorScheme(_colours, WXSIZEOF(_colours)))
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
}
const wxString& PieChart::GetName() const { return _name; }

Chart* PieChart::Create(vector<double>& data, vector<string>& categories)
{
    // first step: create plot
    if (!_plot)
        _plot = new PiePlot();

    // create dataset
    wxString pie_categories[categories.size()];

    for (int i = 0; i < categories.size(); i++)
    {
        pie_categories[i] = wxString(categories[i]);
    }
    double pie_data[data.size()];
    copy(data.begin(), data.end(), pie_data);

    if (categories.size() > MAX_CATEGORIES)
    {
        string msg = "Max number of asset " + to_string(MAX_CATEGORIES) +
                     " is reached! Please contact the developer";
        wxLogWarning(msg.c_str());
    }
    Update(pie_data, pie_categories, data.size());
    // set legend to plot
    _plot->SetLegend(new Legend(wxBOTTOM, wxCENTER));
    // and finally create chart
    return new Chart(_plot, GetName());
}

void PieChart::Update(double* data, wxString* categories, uint size)
{
    if (!_dataset)
        _dataset = new CategorySimpleDataset(categories, size);

    // and add serie to it
    _dataset->AddSerie(_("Serie 1"), data, size);

    // create category renderer for legend drawing
    _dataset->SetRenderer(new CategoryRenderer(*_colorScheme));

    // set color scheme
    _plot->SetColorScheme(_colorScheme);

    _plot->SetDataset(_dataset);
}