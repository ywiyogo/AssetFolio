// Author: YWiyogo
// Descr.: The asset description

#include "Asset.h"
#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"
#include <fstream>
#include <future>
#include <iostream>
#include <sstream>
mutex Asset::_mtx;

const map<string, Asset::Type> Asset::_typeMap = {
    {"ETF", Type::ETF},
    {"Stock", Type::Stock},
    {"Bond", Type::Bond},
    {"Real Estate", Type::Real_Estate},
    {"Crypto", Type::Crypto},
    {"Commodity", Type::Commodity},
    {"Certificate", Type::Certificate}};

const map<string, Asset::Transaction> Asset::_transactionMap = {
    {"Buy", Asset::Transaction::Buy},
    {"Sell", Asset::Transaction::Sell},
    {"ROI", Asset::Transaction::ROI}};

Asset::Asset(string id, string name, Type type)
    : _id(id), _name(name), _type(type), _amount(0), _balance(0), _avg_price(0),
      _curr_price(0), _curr_value(0), _diff(0), _diff_in_percent(0), _return(0),
      _return_in_percent(0), _profit_loss(0), _return_years(), _rois()
{
}

Asset::~Asset() {}

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
        _profit_loss = value_incl_fees - (amount * _avg_price);
        _amount = _amount - amount;
        if (_amount < 0)
            throw std::runtime_error("Invalid transaction, selling amount is more than the existing amount!");
        if (_amount == 0)
        {
            _avg_price = 0;
            _balance = 0;
        }
        else
        {
            _balance = _balance - value_incl_fees;
            // only update average price if balance more than 0
            if (_balance > 0)
                _avg_price = _balance / _amount;
        }
        updateYearlyReturn(reg_date, _balance, 0);

        updateYearlyRoi(reg_date, _profit_loss);
    }
    else if (transaction == Transaction::ROI)
    {
        _return = _return + value_incl_fees;
        _return_in_percent = _return / _balance * 100;
        updateYearlyReturn(reg_date, _balance, value_incl_fees);
        updateYearlyRoi(reg_date, value_incl_fees);
    }
    else
    {
        cout << "Error: Unknown transaction" << endl;
    }
}

void Asset::updateYearlyReturn(time_t reg_date, float total_value,
                               float returns)
{
    struct tm *tmp = gmtime(&reg_date);
    int register_year = tmp->tm_year + 1900;
    bool isYearFound = false;

    // insert to the return list
    for (map<int, YearlyReturn>::iterator it = _return_years.begin();
         it != _return_years.end(); ++it)
    {
        if (register_year == it->first)
        {
            it->second._total_return += returns;
            if (total_value > 0)
            {
                it->second._total_value = total_value;
                it->second._return_in_percent =
                    it->second._total_return / it->second._total_value * 100;
            }
            else
            {
                it->second._total_value = 0;
                it->second._return_in_percent = 0;
            }
            isYearFound = true;
            break;
        }
    }
    if (!isYearFound)
    { // create a new entry of the year
        YearlyReturn year_returns(register_year, total_value, returns,
                                  returns / total_value * 100.);
        _return_years.insert(
            pair<int, YearlyReturn>(register_year, year_returns));
    }
}

void Asset::updateYearlyRoi(time_t reg_date, float value)
{
    struct tm *tmp = gmtime(&reg_date);
    string date = to_string(tmp->tm_mday) + "." + to_string(tmp->tm_mon + 1) + "." + to_string(tmp->tm_year + 1900);
    map<time_t, float>::iterator find_it = _rois.find(reg_date);
    if (find_it != _rois.end())
    {
        find_it->second += value;
    }
    else
    {
        time_t newtime = reg_date;
        _rois.insert(make_pair(newtime, value));
    }
}

const map<time_t, float> &Asset::getRois()
{
    for (auto it = _rois.begin(); it != _rois.end(); it++)
    {
        struct tm *tmp = gmtime(&it->first);
        string date = to_string(tmp->tm_mday) + "." + to_string(tmp->tm_mon + 1) + "." + to_string(tmp->tm_year + 1900);
    }
    return _rois;
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
float Asset::getProfitLoss() const { return _profit_loss; }
Asset::Type Asset::getType() const { return _type; }
void Asset::setCurrPrice(float price)
{
    _curr_price = price;
    _curr_value = _curr_price * _amount;
    _diff = _curr_price - _avg_price;
    if (_amount > 0)
    {
        _diff_in_percent = _diff / _avg_price * 100;
        _return += _diff * _amount;
        if (_balance > 0)
            _return_in_percent = _return / _balance * 100;
        else
        { // in case of selling part of the asset with more than 100% profit
            _return_in_percent = _return / _avg_price * 100;
        }
    }
    else
    { // All of the asset has been sold
        _diff_in_percent = 0;
        _return_in_percent = 0;
    }
}
