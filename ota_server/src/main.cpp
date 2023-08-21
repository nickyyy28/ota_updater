#include <iostream>
#include <siem/Logger/Logger.h>
#include <siem/Thread/Thread.h>

int main(int argc, char** argv) {
    std::cout << "Hello, World!" << std::endl;
    INFO() << "Hello, World!";
    // GET_LOG_BY_NAME(ota_server) << "Hello, World!";
    return 0;
}