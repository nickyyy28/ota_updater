set(libsiem_path ${PROJECT_SOURCE_DIR}/github/Siempre)

message("libsiem_path: ${libsiem_path}")
message("build command:\n")
message("[ -d build ] || mkdir build && cd build && cmake .. -DCMAKE_INSTALL_PREFIX=${PROJECT_SOURCE_DIR}/3rd_party && make && make install")

add_custom_target(libsiem
    COMMAND [ -d build ] || mkdir build && cd build && cmake .. -DCMAKE_INSTALL_PREFIX=${PROJECT_SOURCE_DIR}/3rd_party && make && make install
    WORKING_DIRECTORY ${libsiem_path}
    COMMENT "Building libsiem"
)

include_directories(${PROJECT_SOURCE_DIR}/3rd_party/include)
link_directories(${PROJECT_SOURCE_DIR}/3rd_party/lib)