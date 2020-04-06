#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>
#include <vector>
class Config
{
  public:
    // static const int PARAM1;
    // The periode of the REST API updater in sec
    static const unsigned int UPDATE_PERIODE;
    static std::vector<std::string> TRANSACTION_COL_NAMES;
    static std::vector<std::string> WATCHLIST_COL_NAMES;
};

#endif