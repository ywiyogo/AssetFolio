## Introduction

Assetfolio is a multi asset portfolio tracker which keeps our assets private. In the world of the cloud technology, many data and information are shared and concentrate to the big technology companies, banks or the data trader companies. Their activities how to share our data are not transparent. Assetfolio application helps us to keep or asset information private on our personal computer. It allows us to retrieve information online about the price changes without sharing our asset data.

Software Dependencies:

* RapidJSON
* C++Request
* wxWidgets
* libcurl

REST-API Provider:
* Without API key: [financialmodelingprep](https://financialmodelingprep.com/developer/docs/)
* With API key: [AlphaVantage](https://www.alphavantage.co/documentation/)

## Getting Started

Installing the dependency libraries:

sudo dnf install libcurl-devel wxGTK3-devel

### Installing WxWidget



### Build the Project

```
mkdir build && cd build
cmake ..
make
./Assetfolio
```

## Lesson Learned

### Concurreny and MessageQueue

The challange in this application is how to manage the running concurrent async tasks for the main GUI task, the GUI updater task and several API request tasks.

The message queue is implemented with the template design, and thus the implementation is written in the header file, see [1](https://stackoverflow.com/a/495056).
This application send a disconnect message to end the infinite loop in the async updater task on the GUI side.

### REST API Communication
Implementing the REST API communication in C++ is very challenging. The reason is because the modern C++ STL doesn't contain a standard library for the REST API communication. After my research I found these following C++ library, with its current drawbacks:

1. POCO: too big for my case, and it needs several minutes to build.
2. C++ REST SDK: needs boost
3. curlpp: confusing concept with `Easy` and `Cleanup`.
4. Pistache: actually I like this lib and currently my favorite. However, currently it doesn't support Windows yet.
5. Served: needs boost

Then, I found out that actually I don't need such a complete networking library. Instead, I only need sending a HTTP(S) request and retrieve the response from a web server. I found the C++ Requests library which is very suitable for this application.

### WxWidget GUI

For a beginner, building a GUI application with wxWidget without a builder/creator tool is very challenging. The wxFormBuilder has made my DUI design more convenient than building from stratch without a builder tool.

### Getting the Toolbar Icons

The material design icons are downloaded from https://materialdesignicons.com/.

## License

Licensed under the [MIT License](LICENSE)
