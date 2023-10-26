#include "iap.h"
#include "ring_buffer.h"
#include "cmsis_os.h"
#include "usart.h"
#include "bsp_crc.h"

static uint8_t iap_rx_buffer[IAP_UART_RX_BUFFER_SIZE] = {0};
static uint16_t send_seq = 0;

#define SEND_IAP_MSGID_TO_SERVICE(msg_id, function) \
    { .MSG_ID = msg_id, .service = function }
#define RECV_IAP_MSGID_TO_SERVICE(msg_id, function) \
    { .MSG_ID = msg_id, .service = function }

static uint8_t RecvResponseKeepAlive(uint32_t msg_id, PacketBuffer* buffer);
static uint8_t RecvResponseNewFirmwareInfo(uint32_t msg_id, PacketBuffer* buffer);
static uint8_t RecvResponseTheNPacket(uint32_t msg_id, PacketBuffer* buffer);
static uint8_t RecvResponseNowUTCTime(uint32_t msg_id, PacketBuffer* buffer);
static uint8_t RecvRequestOldFirmwareInfo(uint32_t msg_id, PacketBuffer* buffer);

static inline uint8_t SendRequestKeepAlive(uint32_t msg_id, PacketBuffer* buffer);
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

static void sendService(uint32_t msg_id)
{
    static uint8_t send_buff[512] = {0};
    struct FRAME_HEAD frame_head = {0};
    struct DATA_HEAD data_head = {0};
    struct DATA_TAIL data_tail = {0};
    uint16_t index = 0;
    uint16_t crc16 = 0xFFFF;
    frame_head.header1 = 0xAB;
    frame_head.header2 = 0xCD;
    frame_head.transport_type = 0;
    frame_head.data_length = 0;
    frame_head.crc8 = CRC8_calculate(&frame_head, sizeof(frame_head) - 1, 0xFF);
    crc16 = CRC16_calculate(&frame_head, sizeof(frame_head), crc16);
    memcpy(send_buff + index, &frame_head, sizeof(frame_head));
    index += sizeof(frame_head);

    data_head.msg_id = msg_id;
    data_head.SEQ = send_seq++;
}

static inline uint8_t SendRequestKeepAlive(uint32_t msg_id, PacketBuffer* buffer)
{
    return 0;
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