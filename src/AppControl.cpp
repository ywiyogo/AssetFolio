#include "AppControl.h"

#include "Stock.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "wx/log.h"
#include <algorithm>
#include <fstream>
#include <iostream>

using namespace std;

AppControl::AppControl()
    : _jsonDoc(make_shared<rapidjson::Document>()), _isUpdateActive(false),
      _apikey(""), _update_freq(30) // 30sec
{
}
AppControl::~AppControl() {}

bool AppControl::isApiKeyEmpty()
{
    return _apikey.compare("") == 0 ? true : false;
}

void AppControl::setApiKey(string key)
{
    _apikey = key;
    ofstream keyfile;
    keyfile.open("../data/api.key");
    keyfile << key;
    keyfile.close();
}

void AppControl::readApiKey()
{
    ifstream keyfile("../data/api.key");
    if (keyfile.is_open())
    {
        getline(keyfile, _apikey);
        keyfile.close();
    }
    else
        cout << "Unable to open file";
}
bool AppControl::readLocalRapidJson(const char* filePath, vector<string>& column_names)
{
    FILE* fp = fopen(filePath, "rb"); // non-Windows use "r"
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    bool isValid = false;

    fclose(fp);
    // expect an JSON object format, not JSON array

    _jsonDoc->ParseStream(is);
    if (_jsonDoc->IsObject() && _jsonDoc->GetObject().HasMember("Activities") &&
        _jsonDoc->GetObject()["Activities"].IsArray())
    {
        isValid = true;
    }
    else
    {
        cout << "JSON not valid" << endl << flush;
    }

    if (isValid)
    {
        auto json_act = _jsonDoc->GetObject()["Activities"].GetArray();

        // creating all asset objects
        for (int i = 0; i < json_act.Size(); i++)
        {
            string strdate = json_act[i]["Date"].GetString();
            struct tm tm;
            strptime(strdate.c_str(), "%d.%m.%Y", &tm);
            time_t date = mktime(&tm);
            string name = json_act[i]["Name"].GetString();
            string id = json_act[i]["ID"].GetString();

            Asset::Type asset_type =
                Asset::_typeMap.at(json_act[i]["AssetType"].GetString());

            string type = json_act[i]["Type"].GetString();

            Asset::Transaction transact_type = Asset::_transactionMap.at(type);
            float amount = json_act[i]["Amount"].GetFloat();
            float price = json_act[i]["Transaction"].GetFloat();

            bool isExist = false;
            for (auto&& it = _assets.begin(); it != _assets.end(); it++)
            {
                if (it->get()->getId() == id)
                {
                    isExist = true;
                    it->get()->registerTransaction(transact_type, date, amount,
                                                   price);
                    break;
                }
            }
            if (!isExist)
            { // Differentiate the equity asset with the others
                if (asset_type == Asset::Type::Stock ||
                    asset_type == Asset::Type::ETF ||
                    asset_type == Asset::Type::Bond)
                {
                    unique_ptr<Stock> stock = make_unique<Stock>(id, name);
                    stock->registerTransaction(transact_type, date, amount,
                                               price);
                    _assets.emplace(move(stock));
                }
                else
                {
                    unique_ptr<Asset> asset =
                        make_unique<Asset>(id, name, asset_type);
                    asset->registerTransaction(transact_type, date, amount,
                                               price);
                    _assets.emplace(move(asset));
                }
            }
        }
        readApiKey();
    }
    return isValid;
}

shared_ptr<rapidjson::Document> AppControl::getJsonDoc() const
{
    return _jsonDoc;
}

set<unique_ptr<Asset>> const& AppControl::getAssets() const { return _assets; }

void AppControl::launchAssetUpdater()
{
    // Start the update tasks in each asset object. This function is called by
    // AppGui BtnOnWatchlist Check first for an internet connection
    _isUpdateActive = true;
    if (_msg_queue.size() > 0)
    {
        _msg_queue.clear();
    }
    for (auto&& it = _assets.begin(); it != _assets.end(); it++)
    {
        // request Asset update to the current intersection (using async)
        _futures.emplace_back(async(&Asset::update, it->get(), ref(_msg_queue),
                                    ref(_isUpdateActive), _update_freq));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Stopping all the running update task
void AppControl::stopUpdateTasks()
{
    if (!_isUpdateActive)
    {
        return;
    }
    else
    {
        _isUpdateActive = false;
    }
    // send a disconnect message to stop the wile loop of the GUI async
    unique_ptr<UpdateData> upd(new UpdateData("disconnect", 0, 0, 0, 0, 0, 0));
    _msg_queue.clear();
    _msg_queue.send(move(upd));

    std::for_each(_futures.begin(), _futures.end(), [](std::future<void>& ftr) {
        auto status = ftr.wait_for(std::chrono::milliseconds(100));
        if (status == std::future_status::timeout ||
            status == std::future_status::deferred)
        {
            std::cout << "   stopUpdateTasks timeout" << std::endl;
        }
    });
}
// Wait for update function for the GUI application
unique_ptr<UpdateData> AppControl::waitForUpdate()
{
    return _msg_queue.waitForUpdate();
}
