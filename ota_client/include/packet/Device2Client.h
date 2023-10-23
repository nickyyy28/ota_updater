#ifndef OTA_DEVICE_2_CLIENT_PACKET_H
#define OTA_DEVICE_2_CLIENT_PACKET_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

struct FRAME_HEAD {
    uint8_t header1;		//0xAB
    uint8_t header2;		//0xCD
    uint8_t transport_type;	//0为控制消息，1为数据消息
    uint32_t data_length;	//数据帧
    uint8_t crc8;           //帧头CRC8
}__attribute__((packed));

struct DATA_HEAD {
    uint16_t SEQ;           //序列号，每发送一帧自增
    uint16_t msg_id;        //消息ID
}__attribute__((packed));

struct DATA_TAIL {
    uint8_t tail1;          //0xEC
    uint8_t tail2;          //0xED
    uint16_t crc16;         //全包CRC16
}__attribute__((packed));


struct MSG_REQ_FIRMWARE_INFO {  //0x003
    uint32_t frimware_seq;
}__attribute__((packed));

typedef struct MSG_REQ_FIRMWARE_INFO MSG_FIRMWARE_RECV_OK; //0x004

typedef int ZERO_LENGTH_ARRAY[0];

typedef ZERO_LENGTH_ARRAY MSG_CHECK_CLIENT_KEEPALIVE;                     //0x001
typedef ZERO_LENGTH_ARRAY MSG_REQUEST_NEW_FIRMWARE;                       //0x002
typedef ZERO_LENGTH_ARRAY MSG_REQUEST_NOW_UTCTIME;                        //0x005

struct REPORT_CUR_FIRMWARE_INFO {       //0x010
    char device_name[20];       //设备名
    uint32_t version;           //版本信息 n.n.n.n, 每个版本号占据8位,组合成一个四字节数据
    uint32_t update_timestamp;  //更新固件时的时间戳
    char manufacturer[20];      //制造商信息
    char description[30];       //固件描述
    uint32_t size;              //固件大小 单位:Byte
    uint16_t crc16;             //固件整体CRC16
}__attribute__((packed));

union PacketBuffer{
    MSG_CHECK_CLIENT_KEEPALIVE msg_check_client_keepalive;      //0x001
    MSG_REQUEST_NEW_FIRMWARE msg_request_new_firmware;          //0x002
    struct MSG_REQ_FIRMWARE_INFO  msg_req_firmware_info;        //0x003
    MSG_FIRMWARE_RECV_OK    msg_firmware_recv_ok;               //0x004
    MSG_REQUEST_NOW_UTCTIME msg_request_now_utctime;            //0x005
    struct REPORT_CUR_FIRMWARE_INFO report_cur_firmware_info;   //0x010
};

#ifdef __cplusplus
}
#endif

#pragma pack()

#endif //OTA_DEVICE_2_CLIENT_PACKET_H