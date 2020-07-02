// Author: YWiyogo
// Descr.: The application logic

#ifndef APPCONTROL_H_
#define APPCONTROL_H_
#include "Asset.h"
#include "MsgQueue.h"
#include "rapidjson/document.h"
#include <future>
#include <libxml/HTMLparser.h>
#include <memory>
using namespace std;



class Provider
{
  public:
    string _name;
    string _url;
    string _xpath;
    Provider();
    Provider(string name, string url, string xpath);
};

class AppControl
{
  public:
    AppControl(unsigned int upd_freq);
    ~AppControl();

    enum class QueryType
    {
        ISIN,
        SYMBOL
    };

    bool isApiKeyEmpty();

    void setApiKey(string key);

    string getApiKey();

    bool readApiKey();

    bool isEmpty();

    bool readLocalRapidJson(const char* filePath);

    void writeDataToJson(vector<string>& column_names);

    bool saveJson(string savepath);

    bool isAssetTypeValid(string input);
    bool isTransactionTypeValid(string input);

    shared_ptr<rapidjson::Document> getJsonDoc() const;

    shared_ptr<map<string, shared_ptr<Asset>>> getAssets() const;

    unique_ptr<UpdateData> waitForUpdate();

    void calcAllocation(vector<string>& categories, vector<double>& values);
    void calcCurrentAllocation(vector<string>& categories,
                               vector<double>& values);
    void stopUpdateTasks();

    void launchAssetUpdater();

    bool getPriceFromTradegate(vector<unique_ptr<UpdateData>>& updates);

    void clearJsonData();

    static string floatToString(float number, int precision);
    static float stringToFloat(string numstr, int precision);
    rapidjson::Value getQueryType();
    void setQueryType(string type);

    rapidjson::Value getCurrency();
    
    void setCurrency(string currency);

    struct AppException : public exception
    {
        string str;
        AppException(string ss) : str(ss) {}
        ~AppException() throw() {} // Updated
        const char* what() const throw() { return str.c_str(); }
    };
    
    // year, realized RoI
    const map<time_t, float, less<time_t>>& getTotalRealizedRoi();

  private:
    void calcCurrentTotalValues();
    void update(MsgQueue<UpdateData>& msgqueue, bool& isActive,
                unsigned int upd_frequency);
    bool requestFmpApi(vector<unique_ptr<UpdateData>>& updates, string symbols);
    float getExchangeRate(string from, string to);
    void checkJson();

    shared_ptr<rapidjson::Document> _jsonDoc;
    shared_ptr<map<string, shared_ptr<Asset>>> _assets;
    vector<future<void>> _futures;
    MsgQueue<UpdateData> _msg_queue;
    float _total_values;
    bool _isUpdateActive;
    string _api_key;
    unsigned int _update_freq;
    string _currency_ref;
    map<string, shared_ptr<Provider>> _providers;
    map<time_t, float> _accumulated_roi;

};

#endif
