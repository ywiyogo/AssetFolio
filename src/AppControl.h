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
    AppControl();
    ~AppControl();

    bool isApiKeyEmpty();

    void setApiKey(string key);

    void readApiKey();

    bool readLocalRapidJson(const char* filePath, vector<string>& column_names);

    shared_ptr<rapidjson::Document> getJsonDoc() const;

    set<unique_ptr<Asset>> const& getAssets() const;

    unique_ptr<UpdateData> waitForUpdate();

    void stopUpdateTasks();

    void launchAssetUpdater();

  private:
    shared_ptr<rapidjson::Document> _jsonDoc;
    set<unique_ptr<Asset>> _assets;
    vector<future<void>> _futures;
    MsgQueue<UpdateData> _msg_queue;

    bool _isUpdateActive;
    string _apikey;
    uint _update_freq;
};

#endif