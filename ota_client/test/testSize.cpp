#include "packet/Device2Client.h"
#include <iostream>

int main(void)
{
    std::cout << "FRAME_HEAD size: " << sizeof(FRAME_HEAD) << std::endl;
    std::cout << "REPORT_CUR_FIRMWARE_INFO size: " << sizeof(REPORT_CUR_FIRMWARE_INFO) << std::endl;
    return 0;
}