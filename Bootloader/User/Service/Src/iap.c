#include "iap.h"
#include "ring_buffer.h"

#define SEND_IAP_MSGID_TO_SERVICE(msg_id, function) \
    { .MSG_ID = msg_id, .service = function }
#define RECV_IAP_MSGID_TO_SERVICE(msg_id, function) \
    { .MSG_ID = msg_id, .service = function }

static uint8_t RecvResponseKeepAlive(uint32_t msg_id, PacketBuffer* buffer);
static uint8_t RecvResponseNewFirmwareInfo(uint32_t msg_id, PacketBuffer* buffer);
static uint8_t RecvResponseTheNPacket(uint32_t msg_id, PacketBuffer* buffer);
static uint8_t RecvResponseNowUTCTime(uint32_t msg_id, PacketBuffer* buffer);
static uint8_t RecvRequestOldFirmwareInfo(uint32_t msg_id, PacketBuffer* buffer);

static uint8_t SendRequestKeepAlive(uint32_t msg_id, PacketBuffer* buffer);
static uint8_t SendRequestNewFirmwareInfo(uint32_t msg_id, PacketBuffer* buffer);
static uint8_t SendRequestTheNPacket(uint32_t msg_id, PacketBuffer* buffer);
static uint8_t SendReportNPacketOK(uint32_t msg_id, PacketBuffer* buffer);
static uint8_t SendRequestNowUTCTime(uint32_t msg_id, PacketBuffer* buffer);
static uint8_t SendResponseOldFirmwareInfo(uint32_t msg_id, PacketBuffer* buffer);

static volatile IAP_Send_Service iap_send_service = {
    SEND_IAP_MSGID_TO_SERVICE(0x001, SendRequestKeepAlive),
    SEND_IAP_MSGID_TO_SERVICE(0x002, SendRequestNewFirmwareInfo),
    SEND_IAP_MSGID_TO_SERVICE(0x003, SendRequestTheNPacket),
    SEND_IAP_MSGID_TO_SERVICE(0x004, SendReportNPacketOK),
    SEND_IAP_MSGID_TO_SERVICE(0x005, SendRequestNowUTCTime),
    SEND_IAP_MSGID_TO_SERVICE(0x010, SendResponseOldFirmwareInfo),
};

static volatile IAP_Recv_Service iap_recv_service = {
    RECV_IAP_MSGID_TO_SERVICE(0x101, RecvResponseKeepAlive),
    RECV_IAP_MSGID_TO_SERVICE(0x102, RecvResponseNewFirmwareInfo),
    RECV_IAP_MSGID_TO_SERVICE(0x103, RecvResponseTheNPacket),
    RECV_IAP_MSGID_TO_SERVICE(0x105, RecvResponseNowUTCTime),
    RECV_IAP_MSGID_TO_SERVICE(0x110, RecvRequestOldFirmwareInfo),
};

static uint8_t SendRequestKeepAlive(uint32_t msg_id, PacketBuffer* buffer)
{

}

static uint8_t SendRequestNewFirmwareInfo(uint32_t msg_id, PacketBuffer* buffer)
{

}

static uint8_t SendRequestTheNPacket(uint32_t msg_id, PacketBuffer* buffer)
{

}

static uint8_t SendReportNPacketOK(uint32_t msg_id, PacketBuffer* buffer)
{

}

static uint8_t SendRequestNowUTCTime(uint32_t msg_id, PacketBuffer* buffer)
{

}

static uint8_t SendResponseOldFirmwareInfo(uint32_t msg_id, PacketBuffer* buffer)
{

}

static uint8_t RecvResponseKeepAlive(uint32_t msg_id, PacketBuffer* buffer)
{

}

static uint8_t RecvResponseNewFirmwareInfo(uint32_t msg_id, PacketBuffer* buffer)
{

}

static uint8_t RecvResponseTheNPacket(uint32_t msg_id, PacketBuffer* buffer)
{

}

static uint8_t RecvResponseNowUTCTime(uint32_t msg_id, PacketBuffer* buffer)
{

}

static uint8_t RecvRequestOldFirmwareInfo(uint32_t msg_id, PacketBuffer* buffer)
{

}

static PacketBuffer recv = {0}, send = {0};
static RingBuffer recv_data;

void iap_loop(void)
{
    ring_buffer_init(&recv_data);
    

}