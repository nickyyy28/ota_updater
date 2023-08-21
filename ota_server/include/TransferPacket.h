#pragma once

#include <string>
#include <cstdint>
#include <fstream>

namespace ota_server {

class FileLoader {
public:
    FileLoader(const std::string& path);
    ~FileLoader();
private:
    uint32_t m_fd;
    std::ifstream m_stream;
};

enum class TransferPacketType {
    DATA,   //data frame
    REQ,    //request frame
    DESC,  //description frame
    START,  //start frame
    END,    //end frame
    ACK,    //ack frame
};

#pragma pack(1)

struct DataPacket {
    uint16_t m_pack_seq;
    uint16_t m_pack_size;
    uint8_t m_pack_data[512];
};

struct HeadPacket {
    uint32_t m_pack_head;
    TransferPacketType m_pack_type;
};

struct TailPacket {
    uint16_t m_crc16;
    uint8_t m_pack_tail[2];
};

#pragma pack()

}