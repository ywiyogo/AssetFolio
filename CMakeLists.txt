cmake_minimum_required(VERSION 3.15)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

set(CMAKE_CXX_STANDARD 17)
add_definitions(-pthread)
set(CMAKE_BUILD_TYPE Debug)

project(Assetfolio VERSION 1.0)

# --------------------------------------
# third party libraries

find_package(wxWidgets REQUIRED COMPONENTS core base adv aui)
include(${wxWidgets_USE_FILE})
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

include_directories(include )    # wxFreeChart
# --------------------------------------
file(GLOB project_SRCS src/*.cpp src/wxFreeChart/*.cpp src/wxFreeChart/**/*.cpp )

add_executable(Assetfolio ${project_SRCS})
target_include_directories(Assetfolio PRIVATE ${wxWidgets_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIRS})
# link libraries to the exec target
target_link_libraries(Assetfolio ${wxWidgets_LIBRARIES} curl cpr ${LIBXML2_LIBRARIES})

