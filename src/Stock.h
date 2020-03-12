#ifndef STOCK_H_
#define STOCK_H_
#include "Asset.h"
#include <map>

class Stock : public Asset
{
  public:
    Stock(string id, string name);
    ~Stock();
    void registerDividend(time_t register_date, double dividend_incl_tax);

  private:
    double _dividend;
};

#endif