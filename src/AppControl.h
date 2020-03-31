// Author: YWiyogo
// Descr.: The application logic

#ifndef APPCONTROL_H_
#define APPCONTROL_H_
#include "Asset.h"
#include "MsgQueue.h"
#include "rapidjson/document.h"
#include <future>
#include <memory>
#include <libxml/HTMLparser.h>
using namespace std;

class AppControl
{
  public:
    AppControl(uint upd_freq);
    ~AppControl();

    enum class QueryType
    {
        ISIN,
        SYMBOL
    };
    static const map<string, QueryType> _idtype_map;

    bool isApiKeyEmpty();

    void setApiKey(string key);

    void readApiKey();

    bool readLocalRapidJson(const char* filePath, vector<string>& column_names);

    void writeDataToJson(vector<string>& column_names);

    bool saveJson(string savepath);
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

    struct AppException : public exception
    {
        string str;
        AppException(string ss) : str(ss) {}
        ~AppException() throw() {} // Updated
        const char* what() const throw() { return str.c_str(); }
    };

    struct Provider{
      string _name;
      string _url;
      unique_ptr<xmlChar> _xpath;
      Provider();
      Provider(string name, string url, unique_ptr<xmlChar> xpath){
        _name = name;
        _url = url;
        _xpath = move(xpath);
      }
    };
  private:
    void calcCurrentTotalValues();
    void update(MsgQueue<UpdateData>& msgqueue, bool& isActive,
                uint upd_frequency);
    bool requestFmpApi(vector<unique_ptr<UpdateData>>& updates,  string symbols);
    float getExchangeRate(string from, string to);
    void checkJson();
    
    shared_ptr<rapidjson::Document> _jsonDoc;
    shared_ptr<map<string, shared_ptr<Asset>>> _assets;
    vector<future<void>> _futures;
    MsgQueue<UpdateData> _msg_queue;
    float _total_values;
    bool _isUpdateActive;
    string _apikey;
    uint _update_freq;
    QueryType _query_type;
    string _currency_ref;
    map<string, shared_ptr<Provider>> _providers;
};

#endif