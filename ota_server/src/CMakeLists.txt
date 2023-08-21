
set(SRC_LIST 
    main.cpp
    TransferPacket.cpp
)

add_executable(ota_server ${SRC_LIST})

include_directories(${PROJECT_SOURCE_DIR}/3rd_party/include)
include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/3rd_party/lib)

SET(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${PROJECT_SOURCE_DIR}/3rd_party/lib)
find_package(libsiem REQUIRED)

if(libsiem_FOUND)
    message("libsiem found...")
    include_directories(${libsiem_INCLUDE_DIR})
else(libsiem_FOUND)
    message(FATAL_ERROR "libsiem not found")
endif(libsiem_FOUND)

set(
    3RD_PARTY
    libsiem::siem_shared
    yaml-cpp
    pthread
)

target_link_libraries(ota_server ${3RD_PARTY})

add_dependencies(ota_server libsiem)