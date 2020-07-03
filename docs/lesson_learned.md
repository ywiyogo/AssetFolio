# Lesson Learned

This section documents the lesson learned of the development process. It aims for the developers, not for the users. My main motivation to start this project was to build a simple scaleable application which applies the following concepts:

* multi-platform build using CMake, one source code for creating an application that runs on Windows 10, all Linux Distribution and macOS.
* the [clean architecture](https://blog.cleancoder.com/uncle-bob/2012/08/13/the-clean-architecture.html) or [onion architecture](https://jeffreypalermo.com/2008/07/the-onion-architecture-part-1/) which separates the core logic, use case logic, controllers and the frameworks. The architecture allows us to switch the GUI frameworks (Qt5 or wxWidget) and the data sources (API or web-crawling) in the application code.
* C++ multi-threading and message pasing.

Any comments and critics regarding those three concepts are welcome.

## Concurrency and MessageQueue

The challenge in this application is how to manage the running concurrent async tasks for the main GUI task, the GUI updater task and several API request tasks.

The message queue is implemented with the template design, and thus the implementation is written in the header file, see [1](https://stackoverflow.com/a/495056).
This application sends a disconnect message to end the infinite loop in the async updater task on the GUI side.

## REST API Communication
Implementing the REST API communication in C++ is very challenging. The reason that the modern C++ STL doesn't contain a standard library for the REST API communication. After some research, these following C++ libraries are taken to consideration, with its current drawbacks:

1. POCO: too big for my case, and it needs several minutes to build.
2. C++ REST SDK: needs boost
3. curlpp: confusing concept with `Easy` and `Cleanup`.
4. Pistache: this library is promising. However, currently, it doesn't support Windows yet.
5. Served: needs boost

The application doesn't need such a complete networking library. Instead, it needs to send a HTTP(S) request and retrieve the response from a web server. Thus, C++ Requests library is very suitable for this application.

## WxWidget GUI

For a beginner, building a GUI application with wxWidget without a builder/creator tool is very challenging. The [wxFormBuilder](https://github.com/wxFormBuilder/wxFormBuilder) has made my GUI design more convenient than building from scratch without a builder tool.

Creating and starting an asynchronous task from the main GUI task leads to an unpredictable program crash during the runtime. Thus, instead of creating a `std::async`, an UpdaterThread class which is derived from `wxThread` is created and called in the `initWatchlistGrid)()`.

For the chart visualization, the application includes the [wxFreeChart](https://iwbnwif.github.io/freechart_docs/html/index.html) components. The source code can be found in [this repository](https://github.com/iwbnwif/wxFreeChart).

## Software Delivery and Software Release

For Windows, several dlls have to be copied to the binary folder.
The library _vcruntime140.dll_ has been obtained from _C:\User\\$(Username)\\.vscode\extensions\ms-vscode.cpptools-0.28.1\debugAdapters\vsdbg\bin\Remote Debugger\x64\vcruntime140.dll_.

For Linux, an AppImage file is generated to serve different Linux distributions.

## Getting the Toolbar Icons

The material design icons are downloaded from https://materialdesignicons.com/.