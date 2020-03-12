#include "Asset.h"
#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"
#include <fstream>
#include <future>
#include <iostream>
mutex Asset::_mtx;

const map<string, Asset::Type> Asset::_typeMap = {
    {"ETF", Type::ETF},
    {"Stock", Type::Stock},
    {"Bond", Type::Bond},
    {"Real Estate", Type::Real_Estate},
    {"Cryptocurency", Type::Cryptocurrency},
    {"Commodity", Type::Commodity},
    {"Certificate", Type::Certificate}};

const map<string, Asset::Transaction> Asset::_transactionMap = {
    {"Buy", Asset::Transaction::Buy},
    {"Sell", Asset::Transaction::Sell},
    {"ROI", Asset::Transaction::ROI}};

Asset::Asset(string id, string name, Type type)
    : _id(id), _name(name), _type(type), _amount(0), _balance(0), _avg_price(0),
      _curr_price(0), _curr_value(0), _diff(0), _diff_in_percent(0), _return(0),
      _return_in_percent(0)
{
}

Asset::~Asset() { cout << _name << " destructor called" << endl << flush; }

void Asset::registerTransaction(Asset::Transaction transaction, time_t reg_date,
                                float amount, float value_incl_fees)
{

    if (transaction == Transaction::Buy)
    {
        _amount = _amount + amount;
        _balance = _balance + value_incl_fees;
        _avg_price = _balance / _amount;
        updateYearlyReturn(reg_date, _balance, 0);
    }
    else if (transaction == Transaction::Sell)
    {
        _amount = _amount - amount;
        _balance = _balance - value_incl_fees;
        _avg_price = _balance / _amount;
        updateYearlyReturn(reg_date, _balance, 0);
    }
    else if (transaction == Transaction::ROI)
    {
        _return = _return + value_incl_fees;
        _return_in_percent = _return / _balance * 100;
        updateYearlyReturn(reg_date, _balance, value_incl_fees);
    }
    else
    {
        cout << "Error: Unknown transaction" << endl;
    }
}

void Asset::updateYearlyReturn(time_t reg_date, float total_value,
                               float returns)
{
    tm local_tm = *localtime(&reg_date);
    int register_year = local_tm.tm_year + 1900;
    bool isYearFound = false;

    // insert to the return list
    for (map<int, YearlyReturn>::iterator it = _return_years.begin();
         it != _return_years.end(); ++it)
    {
        if (register_year == it->first)
        {
            it->second._total_return = +returns;
            it->second._total_value = total_value;
            it->second._return_in_percent =
                it->second._total_return / it->second._total_value * 100;
            isYearFound = true;
            break;
        }
    }
    if (!isYearFound)
    {
        YearlyReturn year_returns(register_year, total_value, returns,
                                  returns / total_value * 100.);
        _return_years.insert(
            pair<int, YearlyReturn>(register_year, year_returns));
    }
}

// void Asset::requestAlphaVantageApi(string symbol, string apikey)
// {
//     // update current price form web api. This function is the worker tasks
//     // called by async create a http REST request using an API key.
//     // std::unique_lock<std::mutex> ulock(_mtx);
//     string url =
//         "https://www.alphavantage.co/query?function=GLOBAL_QUOTE&symbol=" +
//         symbol + "&apikey=" + apikey;

//     auto r = cpr::Get(cpr::Url{url}, cpr::Parameters{{"anon", "true"}});
//     // r.status_code;                  // 200
//     // r.headers["content-type"];      // application/json; charset=utf-8
//     // r.text;                         // JSON text string

//     cout << "Result code: " << r.status_code
//          << "\nHeaders: " << r.header["content-type"] << "\n"
//          << r.text << endl
//          << flush;
//     if (r.status_code == 200)
//     {
//         if (r.header["content-type"].find("application/json") !=
//             std::string::npos)
//         {
//             rapidjson::Document json_response;
//             json_response.Parse(r.text.c_str());
//             _curr_price =
//                 stof(json_response["Global Quote"]["05. price"].GetString());

//             _curr_value = _amount * _curr_price;
//             _diff = _amount * (_curr_price - _avg_price);
//             _diff_in_percent = _diff / _balance * 100;
//         }
//     }
// }

void Asset::runDemo()
{
    _curr_price = _avg_price;
    for (int i = 0; i < 3; i++)
    {
        std::this_thread::sleep_for(std::chrono::seconds(3));

        _curr_price++;
        cout << "Price for asset " << _name << ": " << _curr_price << endl
             << flush;
        _curr_value = _amount * _curr_price;
        _diff = _amount * (_curr_price - _avg_price);
        _diff_in_percent = _diff / _balance * 100;
    }
}

string Asset::getId() const { return _id; }
string Asset::getName() const { return _name; }
float Asset::getAmount() const { return _amount; }
float Asset::getBalance() const { return _balance; }
float Asset::getAvgPrice() const { return _avg_price; }
float Asset::getCurrPrice() const { return _curr_price; }
float Asset::getCurrValue() const { return _curr_value; }
float Asset::getDiff() const { return _diff; }
float Asset::getDiffInPercent() const { return _diff_in_percent; }
float Asset::getReturn() const { return _return; }
float Asset::getReturnInPercent() const { return _return_in_percent; }

void Asset::setCurrPrice(float price)
{
    _curr_price = price;
    _curr_value = _curr_price * _amount;
    _diff = _curr_value - _balance;
    _diff_in_percent = _diff / _balance * 100;
    _return += _diff;
    _return_in_percent = _return / _balance * 100;
}
