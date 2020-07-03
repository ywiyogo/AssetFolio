# AssetFolio

An All-in-One Asset Tracker and Watchlist Application that keeps your asset data private.

---

## Overview

AssetFolio is a **multi-asset portfolio tracker** that keeps our assets **private**. In the world of cloud technology, many data and information are shared and concentrate on the big technology companies, banks or the data trader companies. Their activities on how to share our data are not transparent. AssetFolio application helps us to keep our asset information private in our computer. It allows us to retrieve information online about the price changes without sharing our asset data.

Preview for the activity list using Qt5 and Qt5 pie chart:

![Activity list][activity]


Preview for the watchlist and asset tracker using WxWidget UI Library:

![Watchlist][watchlist]

---

## Installation

Software Dependencies:

* [Qt5](https://doc.qt.io/qt-5/gettingstarted.html) or [wxWidgets](https://www.wxwidgets.org/)for the GUI
* [RapidJSON](https://github.com/Tencent/rapidjson) (submodule)
* [C++ Request](https://github.com/whoshuu/cpr) (submodule) which depends on `libcurl`
* [libxml2](http://www.xmlsoft.org/)

Installing the dependency libraries:

* Windows

    1. Install Git, MinGW w64, Qt5 and [OpenSSl v1.1.1](https://slproweb.com/products/Win32OpenSSL.html)
    2. Clone *vcpkg* to C:/Libs/: `git clone https://github.com/microsoft/vcpkg.git`
    2. Build *vcpkg* by running `.\bootstrap-vcpkg.bat`
    3. Hook-up user-wide integration: `.\vcpkg integrate install`
    3. Install the library packages:
          `.\vcpkg install curl:x64-windows`
          `.\vcpkg install libxml2:x64-windows`
          `.\vcpkg install libssh2:x64-windows`
          `.\vcpkg install zlib:x64-windows`

* Ubuntu

        sudo apt-get install build-essential checkinstall libssl-dev libcurl4-openssl-dev libxml2-dev libgtest-dev <libgtk-3-dev|qt5-default qtcreator>

* Fedora

        sudo dnf install openssl-devel libcurl-devel libxml2-devel gtest-devel <wxGTK3-devel| qt5-qtbase qt5-qtbase-devel qt-creator>

---

## Getting Started 

### Download the Source

1. Clone this repository: `git clone https://github.com/ywiyogo/AssetFolio.git`

2. Updating the submodules `git submodule update --init --recursive`

3. Clone the CPR library in the submodule folder: `cd submodules && git clone https://github.com/whoshuu/cpr.git`.


### Build and Start the Project

Currently, the project supports UNIX and Windows. It shall work also on MAC, however, I don't test it yet.

#### UNIX System
---

Got to the project folder and type these commands:

```
mkdir build && cd build
cmake ..
make
./AssetFolio
```

In order to create an AppImage of the AssetFolio, run these commands after `make`:

```
make install DESTDIR=AppDir
./createAppImage.sh

```

#### Windows System
---

Got to the project folder and type these commands:

```
mkdir build && cd build
cmake .. -G "MinGW MakeFiles"
make
.\bin\AssetFolio.exe
```

Note, change `set(GUI "Qt")` to `set(GUI "Wx")` to build the GUI with WxWidget.

An example dataset in _data/example.json_ can be opened with the "arrow up" toolbar icon.

The C++ Request submodules include GTest to test its code. If we don't want to install GTest in our system, we can disable it in CMakeLists.txt `set(USE_SYSTEM_GTEST OFF)`.


[activity]: imgs/activity.png "Screenshot of the activity list"
[watchlist]: imgs/watchlist.png "Screenshot of the watchlist and the tracker"
