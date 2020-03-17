// Author: YWiyogo
// Descr.: The asset description

#ifndef Asset_H
#define Asset_H

#include "MsgQueue.h"
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
using namespace std;

struct YearlyReturn
{
    int _year;
    float _total_value;
    float _total_return;
    // return in percent
    float _return_in_percent;
    YearlyReturn(int year, float val, float ret, float ret_percent)
        : _year(year), _total_value(val), _total_return(ret),
          _return_in_percent(ret_percent){};
};

class Asset : public std::enable_shared_from_this<Asset>
{
  public:

    enum class Type
    {
        Stock,
        ETF,
        Bond,
        Real_Estate,
        Crypto,
        Commodity,
        Certificate
    };
    enum class Transaction
    {
        Buy,
        Sell,
        ROI
    };
    static const map<string, Type> _typeMap;
    static const map<string, Transaction> _transactionMap;
    // constructor & destructor
    Asset(string id, string name, Type type);
    ~Asset();

    void registerTransaction(Asset::Transaction transaction, time_t reg_date,
                             float amount, float price_incl_fees);

    void updateYearlyReturn(time_t reg_date, float total_value,
                            float returns = 0.);

    void requestAlphaVantageApi(string symbol, string apikey);

    bool requestFmpApi(string symbol);
    void runDemo();

    string getId() const;
    string getName() const;
    float getAmount() const;
    float getBalance() const;
    float getAvgPrice() const;
    float getCurrPrice() const;
    float getCurrValue() const;
    float getDiff() const;
    float getDiffInPercent() const;
    float getReturn() const;
    float getReturnInPercent() const;
    Type getType() const;
    void setCurrPrice(float price);

  protected:
    std::shared_ptr<Asset> get_shared_this() { return shared_from_this(); }

    /* data */
    string _id;
    string _name;
    // asset type
    Type _type;
    // total amount of the asset
    float _amount;
    // total spending
    float _balance;
    // average buying price
    float _avg_price;
    // current price
    float _curr_price;
    // current total value
    float _curr_value;

    // Difference to avg buying price
    float _diff;
    // Difference to avg buying price in percent
    float _diff_in_percent;

    // asset return in percent
    float _return;
    float _return_in_percent;

    // list of the returns with its value and its returns
    map<int, YearlyReturn> _return_years;
    static mutex _mtx;
};

#endif