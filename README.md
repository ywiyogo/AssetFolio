## Introduction

Assetfolio is a multi-asset portfolio tracker that keeps our assets private. In the world of cloud technology, many data and information are shared and concentrate on the big technology companies, banks or the data trader companies. Their activities on how to share our data are not transparent. Assetfolio application helps us to keep or asset information private on our personal computer. It allows us to retrieve information online about the price changes without sharing our asset data.

Software Dependencies:

* [wxWidgets](https://www.wxwidgets.org/) for the GUI
* [RapidJSON](https://github.com/Tencent/rapidjson) (submodule)
* [C++ Request](https://github.com/whoshuu/cpr) (submodule) which depends on `libcurl`
* libxml2

REST-API Provider:
* Without API key: [financialmodelingprep](https://financialmodelingprep.com/developer/docs/)
* With API key: will be done after a feature request

Preview for the activity list:

![Activity list][activity]

Preview for the watchlist and asset tracker:

![Watchlist][watchlist]
## Getting Started

Installing the dependency libraries:

* Ubuntu

        sudo apt-get install libgtk-3-dev build-essential checkinstall libcurl4-openssl-dev libxml2-dev libgtest-dev

* Fedora

        sudo dnf install libcurl-devel wxGTK3-devel libxml2-devel gtest-devel

### Download the Source

1. Clone this repository: `git clone https://github.com/ywiyogo/Assetfolio.git`

2. Updating the submodules `git submodule update --init --recursive`


### Build and Start the Project

```
mkdir build && cd build
cmake ..
make
./Assetfolio
```

An example dataset in _data/example.json_ can be opened with the "arrow up" toolbar icon.

The C++ Request submodules include GTest to test its code. If we don't want to install GTest in our system, we can disable it in CMakeLists.txt `set(USE_SYSTEM_GTEST OFF)`.

### Dataset Format
The application uses the JSON format for saving the transaction activity data. All the user data shall be located in the data folder. The user can see the example.json as the template. Three obligatory member names are `QueryType`, `Currency`, and `Activities`.

An `ID` can be an ISIN or a symbol ticker. The symbol has to be found in https://financialmodelingprep.com/api/v3/company/stock/list or [FMPSymbolList.json](data/FMPSymbolList.json). The `QueryType` can be a `SYMBOL` or `ISIN` which describes how the asset can be updated. Since commodities and cryptocurrency do not have ISIN, the ID of each commodity or cryptocurrency shall be a symbol ticker.

The supported `AssetType` values are

```
Stock
ETF
Bond
Real_Estate
Crypto
Commodity
Certificate
```

For `Type`, the possible values are `Buy, Sell, ROI`. 

The `Transaction` and `Amount` values have to be a number (float).


## Lesson Learned

### Concurrency and MessageQueue

The challenge in this application is how to manage the running concurrent async tasks for the main GUI task, the GUI updater task and several API request tasks.

The message queue is implemented with the template design, and thus the implementation is written in the header file, see [1](https://stackoverflow.com/a/495056).
This application sends a disconnect message to end the infinite loop in the async updater task on the GUI side.

### REST API Communication
Implementing the REST API communication in C++ is very challenging. The reason that the modern C++ STL doesn't contain a standard library for the REST API communication. After some research, these following C++ libraries are taken to consideration, with its current drawbacks:

1. POCO: too big for my case, and it needs several minutes to build.
2. C++ REST SDK: needs boost
3. curlpp: confusing concept with `Easy` and `Cleanup`.
4. Pistache: this library is promising. However, currently, it doesn't support Windows yet.
5. Served: needs boost

The application doesn't need such a complete networking library. Instead, it needs to send a HTTP(S) request and retrieve the response from a web server. Thus, C++ Requests library is very suitable for this application.

### WxWidget GUI

For a beginner, building a GUI application with wxWidget without a builder/creator tool is very challenging. The [wxFormBuilder](https://github.com/wxFormBuilder/wxFormBuilder) has made my GUI design more convenient than building from scratch without a builder tool.

Creating and starting an asynchronous task from the main GUI task leads to an unpredictable program crash during the runtime. Thus, instead of creating a `std::async`, an UpdaterThread class which is derived from `wxThread` is created and called in the `initWatchlistGrid)()`.

For the chart visualization, the application includes the [wxFreeChart](https://iwbnwif.github.io/freechart_docs/html/index.html) components. The source code can be found in [this repository](https://github.com/iwbnwif/wxFreeChart).

### Getting the Toolbar Icons

The material design icons are downloaded from https://materialdesignicons.com/.

## License

Licensed under the [MIT License](LICENSE)



[activity]: imgs/activity.png "Screenshot of the activity list"
[watchlist]: imgs/watchlist.png "Screenshot of the watchlist and the tracker"
