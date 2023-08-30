#include <iostream>
#include <siem/Logger/Logger.h>
#include <siem/Thread/Thread.h>
#include <siem/Thread/ThreadPool.h>

int main(int argc, char** argv) {
    INFO() << "Hello, World!";

    // siem::ThreadPool pool(5);

    // pool.addTask([](){
    //     DEBUG() << "task";
    // });

    // pool.start();

    // GET_LOG_BY_NAME(ota_server) << "Hello, World!";



    return 0;
}