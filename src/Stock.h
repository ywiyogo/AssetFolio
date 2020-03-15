// Author: YWiyogo
// Descr.: The Stock description which is derived from the Asset class to
// specify the stock features

#ifndef STOCK_H_
#define STOCK_H_

#include "Asset.h"
#include <map>

// A stock class that is derived from the asset class
class Stock : public Asset
{
  public:
    Stock(string id, string name);
    ~Stock();

    void registerDividend(time_t register_date, double dividend_incl_tax);

  private:
    Stock();
    double _dividend;
};

#endif