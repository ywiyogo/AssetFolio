cmake_minimum_required(VERSION 3.15)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

set(CMAKE_CXX_STANDARD 17)
add_definitions(-pthread)
set(CMAKE_BUILD_TYPE Debug)
set(GUI "Qt")
project(Assetfolio VERSION 1.0)

# --------------------------------------
# third party libraries

if("${GUI}" STREQUAL "Qt")
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    find_package(Qt5 COMPONENTS Core Widgets Charts REQUIRED)
    find_package(Qt5Charts)
    file(GLOB GUI_SRCS src/qt/*.cpp src/qt/*.ui)

    qt5_add_resources(RESOURCE_FILES resource/resources.qrc)

    # Windows application icon
    if (WIN32)
      set(WINDOWS_RES_FILE ${CMAKE_CURRENT_BINARY_DIR}/resources.obj)
      if (MSVC)
        add_custom_command(OUTPUT ${WINDOWS_RES_FILE}
          COMMAND rc.exe /fo ${WINDOWS_RES_FILE} resources.rc
          WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/win
        )
      else()
        add_custom_command(OUTPUT ${WINDOWS_RES_FILE}
          COMMAND windres.exe resources.rc ${WINDOWS_RES_FILE}
          WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/win
        )
      endif()
    endif()
else()
    find_package(wxWidgets REQUIRED COMPONENTS core base adv aui)
    include(${wxWidgets_USE_FILE})
    include_directories(include)    # wxFreeChart
    file(GLOB GUI_SRCS src/wx/*.cpp src/wxFreeChart/*.cpp src/wxFreeChart/**/*.cpp)
endif()

# C++ Request flags
set(USE_SYSTEM_CURL ON)
set(BUILD_CPR_TESTS OFF)
set(USE_SYSTEM_GTEST ON)
add_subdirectory(submodules/cpr)
# RapidJSON
include_directories(submodules/rapidjson/include)
# Avoid program crash if (RAPID)JSON format is not suitable
# add_compile_definitions(NDEBUG)

find_package(LibXml2 REQUIRED)


# --------------------------------------
file(GLOB SRCS src/*.cpp )

# link libraries to the exec target

if("${GUI}" STREQUAL "Qt")
    add_executable(Assetfolio ${SRCS} ${GUI_SRCS} ${RESOURCE_FILES})
    target_link_libraries(Assetfolio PRIVATE Qt5::Widgets ${Qt5Charts_LIBRARIES} curl cpr pthread ${LIBXML2_LIBRARIES})
else()
    add_executable(Assetfolio ${SRCS} ${GUI_SRCS})
    target_link_libraries(Assetfolio ${wxWidgets_LIBRARIES} curl cpr ${LIBXML2_LIBRARIES})
endif()
target_include_directories(Assetfolio PRIVATE ${wxWidgets_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIRS})
