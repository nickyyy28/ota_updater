#ifndef OTA_DEVICE_2_CLIENT_PACKET_H
#define OTA_DEVICE_2_CLIENT_PACKET_H

#include <stdint.h>

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

typedef MSG_REQ_FIRMWARE_INFO MSG_FIRMWARE_RECV_OK; //0x004

struct REPORT_CUR_FIRMWARE_INFO {       //0x010
    char device_name[20];       //设备名
    uint32_t version;           //版本信息 n.n.n.n, 每个版本号占据8位,组合成一个四字节数据
    uint32_t update_timestamp;  //更新固件时的时间戳
    char manufacturer[20];      //制造商信息
    char description[30];       //固件描述
    uint32_t size;              //固件大小 单位:Byte
    uint16_t crc16;             //固件整体CRC16
}__attribute__((packed));

#endif //OTA_DEVICE_2_CLIENT_PACKET_H