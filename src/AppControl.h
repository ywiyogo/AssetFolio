// Author: YWiyogo
// Descr.: The application logic

#ifndef APPCONTROL_H_
#define APPCONTROL_H_
#include "Asset.h"
#include "MsgQueue.h"
#include "rapidjson/document.h"
#include <future>
#include <memory>

using namespace std;

class AppControl
{
  public:
    AppControl(uint upd_freq);
    ~AppControl();

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

  private:
    void calcCurrentTotalValues();
    void update(MsgQueue<UpdateData>& msgqueue, bool& isActive,
                uint upd_frequency);
    bool requestFmpApi(vector<unique_ptr<UpdateData>>& updates);

    shared_ptr<rapidjson::Document> _jsonDoc;
    shared_ptr<map<string, shared_ptr<Asset>>> _assets;
    vector<future<void>> _futures;
    MsgQueue<UpdateData> _msg_queue;
    float _total_values;
    bool _isUpdateActive;
    string _apikey;
    uint _update_freq;
};

#endif