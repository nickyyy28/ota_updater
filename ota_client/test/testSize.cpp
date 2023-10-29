#include "packet/Device2Client.h"
#include <iostream> 

int main(void)
{
    std::cout << "FRAME_HEAD size: " << sizeof(struct FRAME_HEAD) << std::endl;
    std::cout << "REPORT_CUR_FIRMWARE_INFO size: " << sizeof(struct RESPONSE_CUR_FIRMWARE_INFO) << std::endl;
    std::cout << "header1 alignment: " << alignof(FRAME_HEAD::header1) << std::endl;
    std::cout << "header2 alignment: " << alignof(FRAME_HEAD::header2) << std::endl;
    std::cout << "transport_type alignment: " << alignof(FRAME_HEAD::transport_type) << std::endl;
    std::cout << "data_length alignment: " << alignof(FRAME_HEAD::data_length) << std::endl;
    std::cout << "crc8 alignment: " << alignof(FRAME_HEAD::crc8) << std::endl;
    return 0;
}