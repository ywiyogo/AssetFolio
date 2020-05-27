cmake_minimum_required(VERSION 3.15)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

set(CMAKE_BUILD_TYPE Release)
set(GUI "Qt")

project(AssetFolio VERSION 1.0 LANGUAGES CXX)

# --------------------------------------
# third party libraries

if("${GUI}" STREQUAL "Qt")
    
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    # all cpp source files for Qt
    set(GUI_SRCS 
    src/qt/appgui.cpp
    src/qt/main.cpp
    src/qt/customtableview.cpp
    src/qt/appgui.ui)
    
    # Windows application icon
    if (WIN32)
      set(WINDOWS_RES_FILE ${CMAKE_CURRENT_BINARY_DIR}/resources.obj)
      
      if (MSVC)
        add_custom_command(OUTPUT ${WINDOWS_RES_FILE}
          COMMAND rc.exe /fo ${WINDOWS_RES_FILE} resources.rc
          WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/win
        )
        set(CMAKE_PREFIX_PATH "C:\\Qt\\Qt5.14.1\\5.14.1\\msvc2017_64")
      else()
        add_custom_command(OUTPUT ${WINDOWS_RES_FILE}
          COMMAND windres.exe resources.rc ${WINDOWS_RES_FILE}
          WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/win
        )
        set(CMAKE_PREFIX_PATH "C:\\Qt\\Qt5.14.1\\5.14.1\\mingw73_64")
      endif()
    endif()
    find_package(Qt5 COMPONENTS Core Widgets Charts REQUIRED)
    qt5_add_resources(RESOURCE_FILES resource/resources.qrc)
    
else()
    find_package(wxWidgets REQUIRED COMPONENTS core base adv aui)
    include(${wxWidgets_USE_FILE})
    include_directories(include)    # wxFreeChart
    file(GLOB GUI_SRCS src/wx/*.cpp src/wxFreeChart/*.cpp src/wxFreeChart/**/*.cpp)
endif()

find_package(Threads REQUIRED)
# C++ Request flags
set(BUILD_CPR_TESTS OFF CACHE BOOL "Don't build cpr tests")

# RapidJSON
add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE submodules/rapidjson/include)
# Avoid program crash if (RAPID)JSON format is not suitable
# add_compile_definitions(NDEBUG)

set(SRCS
    src/AppControl.cpp
    src/Asset.cpp
    src/Config.cpp
    src/Stock.cpp
)

set(USE_SYSTEM_CURL ON CACHE BOOL "Use the system curl for faster builds")


if (WIN32)
    if(MSVC)
      add_compile_options(/W3)
    elseif(MINGW)
      add_compile_options(-Wall -Wextra -Wpedantic)
    endif()
    
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

    set(CURL_DIR "C:/Libs/vcpkg/installed/x64-windows/share/curl/")
    set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-Win64")
    set(OPENSSL_LIBRARIES
    "C:/Program Files/OpenSSL-Win64/lib/libcrypto.lib"
    "C:/Program Files/OpenSSL-Win64/lib/libssl.lib")
    find_package(CURL CONFIG REQUIRED)
    add_subdirectory(submodules/cpr)

    add_library(iconv INTERFACE)
    target_include_directories(iconv INTERFACE "C:/Libs/vcpkg/packages/libiconv_x64-windows/include")

    find_package(Libssh2 REQUIRED)
    
    message(STATUS "No CMake Config for LibXml2: set it manually")
    set(LIBXML2_INCLUDE_DIR "C:/Libs/vcpkg/packages/libxml2_x64-windows/include")
    set(LIBXML2_LIBRARY "C:/Libs/vcpkg/packages/libxml2_x64-windows/lib/libxml2.lib")

    if("${GUI}" STREQUAL "Qt")
        add_executable(AssetFolio ${SRCS} ${GUI_SRCS} ${RESOURCE_FILES})
        target_include_directories(AssetFolio PRIVATE ${LIBXML2_INCLUDE_DIR} src/qt)
        target_link_libraries(AssetFolio PRIVATE Threads::Threads Qt5::Widgets Qt5::Charts cpr rapidjson ${LIBXML2_LIBRARY} iconv)
        
        add_custom_command(TARGET AssetFolio POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt5::Widgets>
            $<TARGET_FILE:Qt5::Core>
            $<TARGET_FILE:Qt5::Charts>
            $<TARGET_FILE:Qt5::Gui>
            "C:/Qt/Qt5.14.1/5.14.1/mingw73_64/bin/libgcc_s_seh-1.dll"
            "C:/Qt/Qt5.14.1/5.14.1/mingw73_64/bin/libstdc++-6.dll"
            "C:/Qt/Qt5.14.1/5.14.1/mingw73_64/bin/libwinpthread-1.dll"
            $<TARGET_FILE_DIR:AssetFolio>)
        
        file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin/platforms/")

        add_custom_command(TARGET AssetFolio POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/Qt/Qt5.14.1/5.14.1/mingw73_64/plugins/platforms/qdirect2d.dll"
            "C:/Qt/Qt5.14.1/5.14.1/mingw73_64/plugins/platforms/qminimal.dll"
            "C:/Qt/Qt5.14.1/5.14.1/mingw73_64/plugins/platforms/qoffscreen.dll"
            "C:/Qt/Qt5.14.1/5.14.1/mingw73_64/plugins/platforms/qwebgl.dll"
            "C:/Qt/Qt5.14.1/5.14.1/mingw73_64/plugins/platforms/qwindows.dll"
            "$<TARGET_FILE_DIR:AssetFolio>/platforms")
        
        if (CMAKE_BUILD_TYPE STREQUAL "DEBUG")
            add_custom_command ( TARGET AssetFolio POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/lib/libcurl-d.dll"
            $<TARGET_FILE_DIR:AssetFolio>)
        endif()

    elseif("${GUI}" STREQUAL "Wx")
        message("Wx Widget for Windows is not supported yet")
    else()
        message(ERROR " GUI Platform configuration is not supported")
    endif()

    # Copy the dll to the executable folder
    add_custom_command ( TARGET AssetFolio POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "C:/Libs/vcpkg/packages/libxml2_x64-windows/bin/libxml2.dll"
    "C:/Libs/vcpkg/packages/curl_x64-windows/bin/libcurl.dll"
    "C:/Libs/vcpkg/packages/liblzma_x64-windows/bin/lzma.dll"
    "C:/Libs/vcpkg/packages/libiconv_x64-windows/bin/libiconv.dll"
    "C:/Libs/vcpkg/packages/libiconv_x64-windows/bin/libcharset.dll"
    "C:/Libs/vcpkg/packages/zlib_x64-windows/bin/zlib1.dll"
    "C:/Windows/SysWOW64/vcruntime140.dll"
    $<TARGET_FILE_DIR:AssetFolio>)

else()#UNIX
    add_compile_options(-Wall -Wextra -Wpedantic)
    find_package(CURL REQUIRED)
    add_subdirectory(submodules/cpr)

    find_package(LibXml2 REQUIRED)
    
    if("${GUI}" STREQUAL "Qt")
        add_executable(AssetFolio ${SRCS} ${GUI_SRCS} ${RESOURCE_FILES})
        # The next line is needed for the Qt custom classes
        target_include_directories(AssetFolio PRIVATE src/qt)
        target_link_libraries(AssetFolio PRIVATE Threads::Threads Qt5::Widgets Qt5::Charts rapidjson cpr LibXml2::LibXml2 )
    elseif("${GUI}" STREQUAL "Wx")
        add_executable(AssetFolio ${SRCS} ${GUI_SRCS})
        target_include_directories(AssetFolio PRIVATE ${wxWidgets_INCLUDE_DIRS})
        target_link_libraries(AssetFolio Threads::Threads ${wxWidgets_LIBRARIES} curl rapidjson cpr LibXml2::LibXml2)
    endif()
    install(FILES resource/AssetFolio.desktop DESTINATION .)
endif()

# Require c++14, better than setting CMAKE_CXX_STANDARD because it won't pollute other targets
# Set the CC and CXX compiler in the environment variable (unix: .bash_profile)
target_compile_features(AssetFolio PRIVATE cxx_std_17)
# placing the binary in the bin folder of the destination
install(TARGETS AssetFolio DESTINATION bin)



