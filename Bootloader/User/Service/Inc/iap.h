#ifndef __IAP_H
#define __IAP_H

#include "Device2Client.h"

#define IAP_UART_RX_BUFFER_SIZE     512

struct RECV_MSGID2Service{
    uint32_t MSG_ID;
    uint8_t (*service)(uint32_t msg_id, PacketBuffer *recv_buffer);
};

struct SEND_MSGID2Service{
    uint32_t MSG_ID;
    uint8_t (*service)(uint32_t msg_id, PacketBuffer *send_buffer);
};

#define RECV_IAP_MAP(msg_id, name)  struct RECV_MSGID2Service name
#define SEND_IAP_MAP(msg_id, name)  struct SEND_MSGID2Service name

typedef struct{
    RECV_IAP_MAP(0x101, onRecvResponseKeepAlive);
    RECV_IAP_MAP(0x102, onRecvResponseNewFirmwareInfo);
    RECV_IAP_MAP(0x103, onRecvResponseTheNPacket);
    RECV_IAP_MAP(0x105, onRecvResponseNowUTCTime);
    RECV_IAP_MAP(0x110, onRecvRequestOldFirmwareInfo);
}IAP_Recv_Service;



typedef struct{
    SEND_IAP_MAP(0x001, onSendRequestKeepAlive);
    SEND_IAP_MAP(0x002, onSendRequestNewFirmwareInfo);
    SEND_IAP_MAP(0x003, onSendRequestTheNPacket);
    SEND_IAP_MAP(0x004, onSendReportNPacketOK);
    SEND_IAP_MAP(0x005, onSendRequestNowUTCTime);
    SEND_IAP_MAP(0x010, onSendResponseOldFirmwareInfo);
}IAP_Send_Service;

#undef RECV_IAP_MAP
#undef SEND_IAP_MAP

void iap_loop(void);

void iap_send_test(void);

#endif //__IAP_H