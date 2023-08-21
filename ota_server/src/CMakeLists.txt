
set(SRC_LIST 
    main.cpp
)

add_executable(ota_server ${SRC_LIST})

add_dependencies(ota_server ${libSiem_Name})