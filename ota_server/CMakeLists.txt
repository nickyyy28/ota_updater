# 工程名为ota_server, 描述为一个简单的ota升级服务器
project(ota_server VERSION 1.0.0 DESCRIPTION "A simple ota server")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# set cpp level cpp11
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(libsiem)

include_directories(${PROJECT_SOURCE_DIR}/include)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
