#include "iap.h"
#include "ring_buffer.h"
#include "cmsis_os.h"
#include "usart.h"
#include "bsp_crc.h"

static uint8_t iap_rx_buffer[IAP_UART_RX_BUFFER_SIZE] = {0};
static uint16_t send_seq = 0;

// #define SEND_IAP_MSGID_TO_SERVICE(msg_id, function) \
//     { .MSG_ID = msg_id, .service = function }
// #define RECV_IAP_MSGID_TO_SERVICE(msg_id, function) \
//     { .MSG_ID = msg_id, .service = function }

// static volatile IAP_Send_Service iap_send_service = {
//     SEND_IAP_MSGID_TO_SERVICE(0x001, SendRequestKeepAlive),
//     SEND_IAP_MSGID_TO_SERVICE(0x002, SendRequestNewFirmwareInfo),
//     SEND_IAP_MSGID_TO_SERVICE(0x003, SendRequestTheNPacket),
//     SEND_IAP_MSGID_TO_SERVICE(0x004, SendReportNPacketOK),
//     SEND_IAP_MSGID_TO_SERVICE(0x005, SendRequestNowUTCTime),
//     SEND_IAP_MSGID_TO_SERVICE(0x010, SendResponseOldFirmwareInfo),
// };

// static volatile IAP_Recv_Service iap_recv_service = {
//     RECV_IAP_MSGID_TO_SERVICE(0x101, RecvResponseKeepAlive),
//     RECV_IAP_MSGID_TO_SERVICE(0x102, RecvResponseNewFirmwareInfo),
//     RECV_IAP_MSGID_TO_SERVICE(0x103, RecvResponseTheNPacket),
//     RECV_IAP_MSGID_TO_SERVICE(0x105, RecvResponseNowUTCTime),
//     RECV_IAP_MSGID_TO_SERVICE(0x110, RecvRequestOldFirmwareInfo),
// };

static uint32_t getPacketSize(uint16_t msg_id)
{
    switch (msg_id) {
        case 0x001: return sizeof(MSG_CHECK_CLIENT_KEEPALIVE);
        case 0x002: return sizeof(MSG_REQUEST_NEW_FIRMWARE);
        case 0x003: return sizeof(struct MSG_REQ_FIRMWARE_INFO);
        case 0x004: return sizeof(MSG_FIRMWARE_RECV_OK);
        case 0x005: return sizeof(MSG_REQUEST_NOW_UTCTIME);
        case 0x010: return sizeof(struct RESPONSE_CUR_FIRMWARE_INFO);
        default: return 0;
    }
}

static uint8_t sendService(uint32_t msg_id, PacketBuffer * buffer)
{
    static uint8_t send_buff[512] = {0};
    struct FRAME_HEAD frame_head = {0};
    struct DATA_HEAD data_head = {0};
    struct DATA_TAIL data_tail = {0};
    uint16_t index = 0;
    uint16_t crc16 = 0xFFFF;
    frame_head.header1 = 0xAB;
    frame_head.header2 = 0xCD;
    if (msg_id == 0x004 || msg_id == 0x010) {
        frame_head.transport_type = 1;
    } else {
        frame_head.transport_type = 0;
    }

    uint32_t data_packet_size = getPacketSize(msg_id);
    if (buffer == NULL && data_packet_size) return 1;
    frame_head.data_length = data_packet_size;
    frame_head.crc8 = CRC8_calculate((uint8_t *)&frame_head, sizeof(frame_head) - 1, 0xFF);
    crc16 = CRC16_calculate((uint8_t *)&frame_head, sizeof(frame_head), crc16);
    memcpy(send_buff + index, &frame_head, sizeof(frame_head));
    index += sizeof(frame_head);

    data_head.msg_id = msg_id;
    data_head.SEQ = send_seq++;

    crc16 = CRC16_calculate((uint8_t *)&data_head, sizeof(data_head), crc16);
    memcpy(send_buff + index, &data_head, sizeof(data_head));
    index += sizeof(data_head);

    if (data_packet_size) {
        crc16 = CRC16_calculate((uint8_t *)buffer, data_packet_size, crc16);
        memcpy(send_buff + index, buffer, data_packet_size);
        index += data_packet_size;
    }
    data_tail.tail1 = 0xEC;
    data_tail.tail2 = 0xED;
    crc16 = CRC16_calculate((uint8_t*)&data_tail, 2, crc16);
    data_tail.crc16 = crc16;
	memcpy(send_buff + index, (uint8_t*)&data_tail, sizeof(data_tail));
    index += sizeof(data_tail);
	
	LOG_DEBUG("send service msgid 0x%04hx total %d bytes", msg_id, index);

    return HAL_UART_Transmit(&huart1, send_buff, index, 1000);
}

uint8_t SendRequestKeepAlive()
{
    return sendService(MSG_ID_SEND_REQUEST_KEEPALIVE, NULL);
}

uint8_t SendRequestNewFirmwareInfo()
{
    return sendService(MSG_ID_SEND_REQUEST_NEW_FIRMWARE_INFO, NULL);
}

uint8_t SendRequestTheNPacket(uint32_t firmware_seq)
{
    static struct MSG_REQ_FIRMWARE_INFO buff = {0};
    buff.frimware_seq = firmware_seq;
    return sendService(MSG_ID_SEND_REQUEST_THE_N_PACKET, (PacketBuffer*)&buff);
}

uint8_t SendReportNPacketOK(uint32_t firmware_seq)
{
    static MSG_FIRMWARE_RECV_OK buff = {0};
    buff.frimware_seq = firmware_seq;
    return sendService(MSG_ID_SEND_REPORT_THE_N_PACKET_OK, (PacketBuffer*)&buff);
}

uint8_t SendRequestNowUTCTime()
{
    return sendService(MSG_ID_SEND_REQUEST_NOW_UTC_TIME, NULL);
}

uint8_t SendResponseOldFirmwareInfo(const char *device_name, uint32_t version, uint32_t update_timestamp, const char *manufacturer, const char *description, uint32_t size, uint16_t crc16)
{
    static struct RESPONSE_CUR_FIRMWARE_INFO buff = {0};
    buff.crc16 = crc16;
    buff.size = size;
    buff.update_timestamp = update_timestamp;
    buff.version = version;
    strcpy(buff.device_name, device_name);
    strcpy(buff.manufacturer, manufacturer);
    strcpy(buff.description, description);
    return sendService(MSG_ID_SEND_RESPONSE_OLD_FIRMWARE_INFO, (PacketBuffer*)&buff);
}

static uint8_t RecvResponseKeepAlive(uint32_t msg_id, PacketBuffer* buffer)
{
    return 0;
}

static uint8_t RecvResponseNewFirmwareInfo(uint32_t msg_id, PacketBuffer* buffer)
{
    return 0;
}

static uint8_t RecvResponseTheNPacket(uint32_t msg_id, PacketBuffer* buffer)
{
    return 0;
}

static uint8_t RecvResponseNowUTCTime(uint32_t msg_id, PacketBuffer* buffer)
{
    return 0;
}

static uint8_t RecvRequestOldFirmwareInfo(uint32_t msg_id, PacketBuffer* buffer)
{
    return 0;
}

static PacketBuffer recv = {0}, send = {0};
static RingBuffer recv_data;

void iap_loop(void)
{
    ring_buffer_init(&recv_data);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    HAL_UART_Receive_DMA(&huart1, iap_rx_buffer, IAP_UART_RX_BUFFER_SIZE);

    while (1) {
        osDelay(20);
    }

}

void iap_send_test(void)
{

}

void USART1_IRQHandler(void)
{
    if (RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE)) {
		__HAL_UART_CLEAR_IDLEFLAG(&huart1);
#ifdef USE_CACHE
		SCB_InvalidateDCache_by_Addr(iap_rx_buffer, IAP_UART_RX_BUFFER_SIZE);
#endif
		HAL_UART_DMAStop(&huart1);
		uint32_t len = IAP_UART_RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
		ring_buffer_write(&recv_data, iap_rx_buffer, len);
        memset(iap_rx_buffer, 0, len);
		HAL_UART_Receive_DMA(&huart1, iap_rx_buffer, IAP_UART_RX_BUFFER_SIZE);
	}
}