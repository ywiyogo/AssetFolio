cmake_minimum_required(VERSION 3.11.3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
set(CMAKE_BUILD_TYPE Debug)

project(Assetfolio)

# Avoid program crash if JSON format is not suitable
# add_compile_definitions(NDEBUG)

find_package(wxWidgets REQUIRED COMPONENTS core base adv aui)
include(${wxWidgets_USE_FILE})
include_directories(./include ../../../yccpp/cpr/include)
LINK_DIRECTORIES(../../../yccpp/cpr/build/lib)
# build the third party library
# file(GLOB LIBRARY_SRCS src/thirdlibs/*.cpp)
# add_library(simdjson STATIC ${LIBRARY_SRCS})

file(GLOB project_SRCS src/*.cpp)

add_executable(Assetfolio ${project_SRCS})
# link libraries to the exec target
target_link_libraries(Assetfolio ${wxWidgets_LIBRARIES} curl cpr)
target_include_directories(Assetfolio PRIVATE ${wxWidgets_INCLUDE_DIRS})
