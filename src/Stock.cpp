// Author: YWiyogo
// Descr.: The Stock description which is derived from the Asset class to
// specify the stock features

#include "Stock.h"
#include <chrono>
Stock::Stock(string id, string name)
    : Asset(id, name, Type::Stock), _dividend(0)
{
}

Stock::~Stock() {  }

void Stock::registerDividend(time_t register_date, double dividend_incl_tax)
{
    _dividend = _dividend + dividend_incl_tax;
    updateYearlyReturn(register_date, 0, dividend_incl_tax);
}