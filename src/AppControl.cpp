// Author: YWiyogo
// Descr.: The application logic

#include "AppControl.h"

#include "Stock.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "wx/log.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <cpr/cpr.h>

using namespace std;
const uint MAX_WRITE_BUFFER = 65536;

AppControl::AppControl(uint upd_freq)
    : _jsonDoc(make_shared<rapidjson::Document>()),
      _assets(make_shared<map<string, shared_ptr<Asset>>>()), _futures(),
      _msg_queue(), _isUpdateActive(false), _apikey(""), _update_freq(upd_freq)
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
bool AppControl::readLocalRapidJson(const char* filePath,
                                    vector<string>& column_names)
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
            // Retrieve the data
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

            // Check if the acquired id has already existed, if not then create
            // a new asset
            if (_assets->find(id) == _assets->end())
            { // Differentiate the equity asset with the others
                if (asset_type == Asset::Type::Stock ||
                    asset_type == Asset::Type::ETF ||
                    asset_type == Asset::Type::Bond)
                {
                    unique_ptr<Stock> stock = make_unique<Stock>(id, name);
                    stock->registerTransaction(transact_type, date, amount,
                                               price);
                    _assets->emplace(stock->getId(), move(stock));
                }
                else
                {
                    unique_ptr<Asset> asset =
                        make_unique<Asset>(id, name, asset_type);
                    asset->registerTransaction(transact_type, date, amount,
                                               price);
                    _assets->emplace(asset->getId(), move(asset));
                }
            }
        }
        readApiKey();
    }
    return isValid;
}

bool AppControl::saveJson(string savepath)
{
    try
    {
        FILE* fp = fopen(savepath.c_str(), "wb");
        char writeBuffer[MAX_WRITE_BUFFER];
        rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
        rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
        _jsonDoc->Accept(writer);
        fclose(fp);
    }
    catch (const exception& e)
    {
        cout << e.what();
        return false;
    }
    return true;
}
shared_ptr<rapidjson::Document> AppControl::getJsonDoc() const
{
    return _jsonDoc;
}

shared_ptr<map<string, shared_ptr<Asset>>> AppControl::getAssets() const
{
    return _assets;
}

void AppControl::calcCurrentTotalValues()
{
    _total_values = 0.;
    // creating all asset objects
    for (auto it = _assets->begin(); it != _assets->end(); it++)
    {
        _total_values += it->second->getCurrValue();
    }
}
void AppControl::calcAllocation(vector<string>& categories,
                                vector<double>& values)
{
    categories.clear();
    values.clear();
    for (auto it = _assets->begin(); it != _assets->end(); it++)
    {
        categories.push_back(it->second->getName());
        values.push_back(static_cast<double>(it->second->getBalance()));
    }
}

void AppControl::calcCurrentAllocation(vector<string>& categories,
                                       vector<double>& values)
{
    categories.clear();
    values.clear();
    calcCurrentTotalValues();
    for (auto it = _assets->begin(); it != _assets->end(); it++)
    {
        categories.push_back(it->second->getName());
        values.push_back(
            static_cast<double>(it->second->getCurrValue() / _total_values));
    }
}

// --------------------------------------------------
// Threads or tasks functions
void AppControl::launchAssetUpdater()
{
    // Start the update tasks in each asset object. This function is called by
    // AppGui BtnOnWatchlist Check first for an internet connection
    _isUpdateActive = true;
    if (_msg_queue.size() > 0)
    {
        _msg_queue.clear();
    }

    // request Asset update
    _futures.emplace_back(async(&AppControl::update, this, ref(_msg_queue),
                                ref(_isUpdateActive), _update_freq));
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
    return move(_msg_queue.waitForUpdate());
}

void AppControl::update(MsgQueue<UpdateData>& msgqueue, bool& isActive,
                        uint upd_frequency)
{
    cout << "AssetUpdate: Start a thread: " << this_thread::get_id() << endl
         << flush;
    // Never use sleep_for more than 100 ms sec to keep the GUI responsive
    std::chrono::time_point<std::chrono::system_clock> stopWatch;
    // init stop watch
    stopWatch = std::chrono::system_clock::now();
    long diffUpdate = upd_frequency;
    vector<unique_ptr<UpdateData>> updates{};
    while (isActive)
    {
        // Add 10 ms delay to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        // compute time difference to stop watch
        if (diffUpdate >= upd_frequency)
        {
            updates.clear();
            if (!requestFmpApi(updates))
            {
                // stop the task if symbol not found to save CPU resource
                return;
            }
            while (updates.size() > 0)
            {
                auto futureTrfLight = async(&MsgQueue<UpdateData>::send,
                                            &msgqueue, move(updates.back()));
                futureTrfLight.wait();
                updates.pop_back();
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
            stopWatch = std::chrono::system_clock::now();
        }
        diffUpdate = std::chrono::duration_cast<std::chrono::seconds>(
                         std::chrono::system_clock::now() - stopWatch)
                         .count();
    }
}

bool AppControl::requestFmpApi(vector<unique_ptr<UpdateData>>& updates)
{
    string symbols = "";
    bool is_first = true;
    for (auto it = _assets->begin(); it != _assets->end(); it++)
    {
        if (is_first)
        {
            symbols += it->first;
            is_first = false;
        }
        else
        {
            symbols += "," + it->first;
        }
    }
    // e.g. https://financialmodelingprep.com/api/v3/quote/ZGUSD,BTCUSD,AAPL
    string url = "https://financialmodelingprep.com/api/v3/quote/" + symbols;
    cout << "URL: " << url << endl << flush;
    auto r = cpr::Get(cpr::Url{url});
    bool is_found = false;
    cout << "Result code: " << r.status_code
         << "\nHeaders: " << r.header["content-type"] << "\n"
         << r.text << endl
         << flush;
    if (r.status_code == 200)
    {
        if (r.header["content-type"].find("application/json") !=
            std::string::npos)
        {
            rapidjson::Document json_resp;
            json_resp.Parse(r.text.c_str());
            if (json_resp.Size() > 0)
            {
                is_found = true;
                for (int i = 0; i < json_resp.Size(); i++)
                { // create an update data and add to the vector reference
                    string id = json_resp[i]["symbol"].GetString();
                    shared_ptr<Asset> asset = _assets->at(id);
                    asset->setCurrPrice(json_resp[i]["price"].GetFloat());

                    unique_ptr<UpdateData> upd_data(new UpdateData(
                        id, asset->getCurrPrice(), asset->getCurrValue(),
                        asset->getDiff(), asset->getDiffInPercent(),
                        asset->getReturn(), asset->getReturnInPercent()));
                    updates.emplace_back(move(upd_data));
                }
            }
        }
    }
    else
    {
        cout << "Request error " << r.status_code << ". " << r.text << endl
             << flush;
    }
    return is_found;
}