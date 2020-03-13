#ifndef APPCONTROL_H_
#define APPCONTROL_H_
#include "Asset.h"
#include "MsgQueue.h"
#include "rapidjson/document.h"
#include <future>
#include <memory>
#include <set>
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

    shared_ptr<rapidjson::Document> getJsonDoc() const;

    shared_ptr<map<string, shared_ptr<Asset>>> getAssets() const;

    unique_ptr<UpdateData> waitForUpdate();

    void stopUpdateTasks();

    void launchAssetUpdater();

  private:
    void update(MsgQueue<UpdateData>& msgqueue, bool& isActive,
                uint upd_frequency);
    bool requestFmpApi(vector<unique_ptr<UpdateData>>& updates);

    shared_ptr<rapidjson::Document> _jsonDoc;
    shared_ptr<map<string, shared_ptr<Asset>>> _assets;
    vector<future<void>> _futures;
    MsgQueue<UpdateData> _msg_queue;

    bool _isUpdateActive;
    string _apikey;
    uint _update_freq;
};

#endif