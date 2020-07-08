#include "Config.h"

const unsigned int Config::UPDATE_PERIODE = 20;

std::vector<std::string> Config::TRANSACTION_COL_NAMES = {};

std::vector<std::string> Config::WATCHLIST_COL_NAMES{
    "ID", "Name", "Amount", "Balance", "Avg Price", "Curr.Price",
    "Curr.Value", "Diff", "Diff %", "Return", "Return %", "Profit"};

std::string Config::DATE_FORMAT= "dd.MM.yyyy";