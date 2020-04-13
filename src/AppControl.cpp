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
#include <algorithm>
#include <cpr/cpr.h>
#include <fstream>
#include <iomanip> // std::setprecision
#include <iostream>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <sstream> // stringstream

using namespace std;
const unsigned int MAX_WRITE_BUFFER = 65536;

const map<string, AppControl::QueryType> AppControl::_querytype_map = {
    {"ISIN", QueryType::ISIN}, {"SYMBOL", QueryType::SYMBOL}};

const map<AppControl::QueryType, string> AppControl::_reverse_querytype_map = {
    {QueryType::ISIN, "ISIN"}, {QueryType::SYMBOL, "SYMBOL"}};

Provider::Provider() {}
Provider::Provider(string name, string url, string xpath)
{
    _name = name;
    _url = url;
    _xpath = xpath;
}

AppControl::AppControl(unsigned int upd_freq)
    : _jsonDoc(make_shared<rapidjson::Document>()),
      _assets(make_shared<map<string, shared_ptr<Asset>>>()), _futures(),
      _msg_queue(), _isUpdateActive(false), _apikey(""), _update_freq(upd_freq),
      _currency_ref("USD")
{
    // Add the HTML data provider
    shared_ptr<Provider> tradegate(make_shared<Provider>(
        "Tradegate", "https://www.tradegate.de/orderbuch.php?lang=en&isin=",
        "//table[@class='full grid noHeadBorder']/tr[2]/td[4]"));
    shared_ptr<Provider> justEtf(make_shared<Provider>(
        "JustETF", "https://www.justetf.com/de/etf-profile.html?isin=",
        "//div[@class='infobox']/div[1]/div[1]/div[1]/span[2]"));
    // insert function of a map doesn't accept a shared_ptr or unique_ptr, use
    // emplace instead
    _providers.emplace(tradegate->_name.c_str(), tradegate);
    _providers.emplace(justEtf->_name.c_str(), justEtf);
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
bool AppControl::readLocalRapidJson(const char *filePath)
{
    FILE *fp = fopen(filePath, "rb"); // non-Windows use "r"
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    fclose(fp);
    // expect an JSON object format, not JSON array
    _jsonDoc->ParseStream(is);

    checkJson();
    _query_type =
        _querytype_map.at(_jsonDoc->GetObject()["QueryType"].GetString());
    _currency_ref = _jsonDoc->GetObject()["Currency"].GetString();

    auto json_act = _jsonDoc->GetObject()["Transactions"].GetArray();

    // creating all asset objects
    for (unsigned int i = 0; i < json_act.Size(); i++)
    {
        // Retrieve the data
        string strdate = json_act[i]["Date"].GetString();
        struct tm tm;
        int yy;
        int mm;
        int dd;
        sscanf(strdate.c_str(), "%d:%d:%d", &yy, &mm, &dd);
        tm.tm_year = yy - 1900; // Years from 1900
        tm.tm_mon = mm - 1;     // Months from January
        tm.tm_mday = dd;
        time_t date = mktime(&tm);
        string name = json_act[i]["Name"].GetString();
        string id = json_act[i]["ID"].GetString();
        Asset::Type asset_type =
            Asset::_typeMap.at(json_act[i]["AssetType"].GetString());
        string type = json_act[i]["Type"].GetString();

        Asset::Transaction transact_type = Asset::_transactionMap.at(type);
        if (!json_act[i]["Amount"].IsNumber())
        {
            throw AppException("JSON Amount of " + id + " has to be a number.");
        }
        float amount = json_act[i]["Amount"].GetFloat();
        if (!json_act[i]["Transaction"].IsNumber())
        {
            throw AppException("JSON Transaction of " + id +
                               " has to be a number.");
        }
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
                stock->registerTransaction(transact_type, date, amount, price);
                _assets->emplace(stock->getId(), move(stock));
            }
            else
            {
                unique_ptr<Asset> asset =
                    make_unique<Asset>(id, name, asset_type);
                asset->registerTransaction(transact_type, date, amount, price);
                _assets->emplace(asset->getId(), move(asset));
            }
        }
    }
    return true;
}

bool AppControl::saveJson(string savepath)
{
    try
    {
        FILE *fp = fopen(savepath.c_str(), "wb");
        char writeBuffer[MAX_WRITE_BUFFER];
        rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
        rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
        _jsonDoc->Accept(writer);
        fclose(fp);
    }
    catch (const exception &e)
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

void AppControl::calcAllocation(vector<string> &categories,
                                vector<double> &values)
{
    categories.clear();
    values.clear();

    for (auto it = _assets->begin(); it != _assets->end(); it++)
    {
        categories.push_back(it->second->getName());
        values.push_back(static_cast<double>(it->second->getBalance()));
    }
}

void AppControl::calcCurrentAllocation(vector<string> &categories,
                                       vector<double> &values)
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

    std::for_each(_futures.begin(), _futures.end(), [](std::future<void> &ftr) {
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

bool AppControl::getPriceFromTradegate(vector<unique_ptr<UpdateData>> &updates)
{
    float curr_price = 0.;
    string currency;
    string fmp_symbols = "";
    bool is_first = true;
    for (auto it = _assets->begin(); it != _assets->end(); it++)
    {
        if (it->second->getType() == Asset::Type::Commodity ||
            it->second->getType() == Asset::Type::Crypto)
        { // collect the symbol first
            if (is_first)
            {
                fmp_symbols += it->first;
                is_first = false;
            }
            else
            {
                fmp_symbols += "," + it->first;
            }
            continue;
        }
        string url = _providers.at("Tradegate")->_url + it->second->getId();
        xmlChar *xpathchar1 =
            (xmlChar *)_providers.at("Tradegate")->_xpath.c_str();
        xmlChar *xpathchar2 = (xmlChar *)"//*[@id='bid']";

        auto r = cpr::Get(cpr::Url{url});
        // cout << "Result code: " << r.status_code
        //      << "\nHeaders: " << r.header["content-type"] << "\n"
        //      << r.text << endl
        //      << flush;

        // Parse HTML and create a DOM tree
        xmlDocPtr doc = htmlReadDoc((xmlChar *)r.text.c_str(), NULL, NULL,
                                    HTML_PARSE_NOWARNING | HTML_PARSE_RECOVER |
                                        HTML_PARSE_NOERROR);

        xmlXPathContextPtr xpath_context = xmlXPathNewContext(doc);
        if (xpath_context == NULL)
        {
            throw AppException("Error in xmlXPathNewContext\n");
        }

        xmlXPathObjectPtr cur_result =
            xmlXPathEvalExpression(xpathchar1, xpath_context);
        xmlXPathObjectPtr result =
            xmlXPathEvalExpression(xpathchar2, xpath_context);
        xmlXPathFreeContext(xpath_context);
        if (cur_result == NULL)
        {
            throw AppException("Error in xmlXPathEvalExpression\n");
        }
        if (result == NULL)
        {
            throw AppException("Error in xmlXPathEvalExpression\n");
        }
        if (xmlXPathNodeSetIsEmpty(cur_result->nodesetval))
        {
            xmlXPathFreeObject(cur_result);
            printf("No result\n");
            continue;
            // return NULL;
        }
        xmlNodeSetPtr currency_nodeset = cur_result->nodesetval;
        if (currency_nodeset->nodeNr > 0)
        {
            xmlChar *curr = xmlNodeListGetString(
                doc, currency_nodeset->nodeTab[0]->xmlChildrenNode, 1);
            stringstream ss;
            ss << curr;
            currency = ss.str();
            xmlFree(curr);
        }

        if (xmlXPathNodeSetIsEmpty(result->nodesetval))
        {
            xmlXPathFreeObject(result);
            printf("No result\n");
            return NULL;
        }
        xmlNodeSetPtr nodeset = result->nodesetval;
        for (int i = 0; i < nodeset->nodeNr; i++)
        {
            xmlChar *keyword = xmlNodeListGetString(
                doc, nodeset->nodeTab[i]->xmlChildrenNode, 1);
            stringstream ss;
            ss << keyword;
            curr_price = stof(ss.str());
            xmlFree(keyword);
        }
        xmlXPathFreeObject(cur_result);
        xmlXPathFreeObject(result);
        xmlFreeDoc(doc);
        xmlCleanupParser();

        shared_ptr<Asset> asset = _assets->at(it->first);
        if (_currency_ref.compare(currency) == 0)
        {
            asset->setCurrPrice(curr_price);
        }
        else
        {
            float rate = getExchangeRate(currency, _currency_ref);
            cout << "Exchange rate: " << rate << endl;
            asset->setCurrPrice(curr_price);
        }

        unique_ptr<UpdateData> upd_data(new UpdateData(
            it->first, asset->getCurrPrice(), asset->getCurrValue(),
            asset->getDiff(), asset->getDiffInPercent(), asset->getReturn(),
            asset->getReturnInPercent()));

        updates.emplace_back(move(upd_data));
    }
    // if FMP symbols exist
    if (fmp_symbols.size() > 2)
        requestFmpApi(updates, fmp_symbols);
    return true;
}
void AppControl::update(MsgQueue<UpdateData> &msgqueue, bool &isActive,
                        unsigned int upd_frequency)
{
    cout << "AssetUpdate: Start a thread: " << this_thread::get_id() << endl
         << flush;
    // Never use sleep_for more than 100 ms sec to keep the GUI responsive
    std::chrono::time_point<std::chrono::system_clock> stopWatch;
    // init stop watch
    stopWatch = std::chrono::system_clock::now();
    unsigned int diffUpdate = upd_frequency;
    vector<unique_ptr<UpdateData>> updates{};
    while (isActive)
    {
        // Add 10 ms delay to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        // compute time difference to stop watch
        if (diffUpdate >= upd_frequency)
        {
            updates.clear();
            string symbols = "";
            bool is_first = true;
            switch (_query_type)
            {
            case QueryType::ISIN:
                getPriceFromTradegate(updates);
                break;
            case QueryType::SYMBOL:

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
                if (!requestFmpApi(updates, symbols))
                {
                    // stop the task if symbol not found to save CPU resource
                    return;
                }
                break;
            default:
                throw AppException(
                    "ID Type invalid, supported ID are ISIN or SYMBOL");
                break;
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

bool AppControl::requestFmpApi(vector<unique_ptr<UpdateData>> &updates,
                               string symbols)
{
    // e.g. https://financialmodelingprep.com/api/v3/quote/ZGUSD,BTCUSD,AAPL
    string url = "https://financialmodelingprep.com/api/v3/quote/" + symbols;
    auto r = cpr::Get(cpr::Url{url});
    bool is_found = false;
    // cout << "Result code: " << r.status_code
    //      << "\nHeaders: " << r.header["content-type"] << "\n"
    //      << r.text << endl
    //      << flush;
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
                for (unsigned int i = 0; i < json_resp.Size(); i++)
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

float AppControl::getExchangeRate(string from, string to)
{
    string symbol;
    bool reverse = false;
    float res = 0.;

    if (to.compare("USD") == 0)
    {
        symbol = from + to;
    }
    else
    {
        if (from.compare("USD") == 0)
            symbol = to + from;
        reverse = true;
    }

    string url = "https://financialmodelingprep.com/api/v3/forex/" + symbol;
    auto r = cpr::Get(cpr::Url{url});
    // cout << "Result code: " << r.status_code
    //      << "\nHeaders: " << r.header["content-type"] << "\n"
    //      << r.text << endl
    //      << flush;
    if (r.status_code == 200)
    {
        if (r.header["content-type"].find("application/json") !=
            std::string::npos)
        {
            rapidjson::Document json_resp;
            json_resp.Parse(r.text.c_str());
            if (!json_resp.HasMember("bid"))
            {
                throw AppException("Currency exchange from " + from + "to " +
                                   to + "is not found");
            }
            string bid = json_resp["bid"].GetString();
            if (!reverse)
                res = stof(bid);
            else
            {
                res = 1. / stof(bid);
            }
        }
    }
    else
    {
        cout << "Request error " << r.status_code << ". " << r.text << endl;
    }
    return res;
}

void AppControl::checkJson()
{
    if (!_jsonDoc->IsObject())
    {
        throw AppException("JSON format is not an object");
    }

    if (!_jsonDoc->GetObject().HasMember("QueryType"))
    {
        throw AppException("JSON format has to have e member 'QueryType'");
    }

    if (!_jsonDoc->GetObject().HasMember("Transactions"))
    {
        throw AppException("JSON format has to have e member 'Transactions'");
    }
    if (!_jsonDoc->GetObject().HasMember("Currency"))
    {
        throw AppException("JSON format has to have e member 'Currency'");
    }

    if (!_jsonDoc->GetObject()["Transactions"].IsArray())
    {
        throw AppException(
            "JSON format member 'Transactions' has to be an array.");
    }
}

void AppControl::clearJsonData()
{
    if (_jsonDoc->IsObject())
        _jsonDoc->RemoveAllMembers();
    _assets->clear();
}

string AppControl::floatToString(float number, int precision)
{
    stringstream stream;
    stream << fixed << setprecision(precision) << number;
    return stream.str();
}
float AppControl::stringToFloat(string numstr, int precision)
{
    stringstream stream;
    stream << fixed << setprecision(precision) << stof(numstr);
    return stof(stream.str());
    ;
}

rapidjson::Value AppControl::getQueryType()
{
    rapidjson::Value query(rapidjson::kStringType);
    query.SetString(_reverse_querytype_map.at(_query_type).c_str(),
                    _reverse_querytype_map.at(_query_type).size());
    return query;
}
rapidjson::Value AppControl::getCurrency()
{
    rapidjson::Value curency(rapidjson::kStringType);
    curency.SetString(_currency_ref.c_str(), _currency_ref.size());
    return curency;
}
