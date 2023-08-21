cmake_minimum_required(VERSION 3.11)
project(ota_updater)

# set external lib
set(OTA_SERVER_ROOT_DIR ${PROJECT_SOURCE_DIR}/ota_server)
set(OTA_SERVER_3RD_PARTY_DIR ${OTA_SERVER_ROOT_DIR}/3rd_party)
set(libSiem_Name "Siempre")
set(libSiem_URL "https://github.com/nickyyy28/Siempre.git")
set(libSiem_Branch "master")
set(libSiem_CONFIGURE_COMMAND "cd ${OTA_SERVER_ROOT_DIR}/github/${libSiem_Name} && mkdir build && cd build && cmake .. -DCMAKE_INSTALL_PREFIX=${OTA_SERVER_3RD_PARTY_DIR} -DBUILD_TYPE=Release")
set(libSiem_MAKE_COMMAND "cd ${OTA_SERVER_ROOT_DIR}/github/${libSiem_Name}/build && make")
set(libSiem_INSTALL_COMMAND "cd ${OTA_SERVER_ROOT_DIR}/github/${libSiem_Name}/build && make install")

# include(ExternalProject)

# ExternalProject_Add(${libSiem_Name}
#     PREFIX ${OTA_SERVER_ROOT_DIR}/github/${libSiem_Name}
#     GIT_REPOSITORY ${libSiem_URL}
#     GIT_BRANCH ${libSiem_Branch}
#     CONFIGURE_COMMAND ${libSiem_CONFIGURE_COMMAND}
#     BUILD_COMMAND ${libSiem_MAKE_COMMAND}
#     INSTALL_COMMAND ${libSiem_INSTALL_COMMAND}
# )
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_subdirectory(ota_server)

