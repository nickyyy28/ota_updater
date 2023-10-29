#include "otaTask.h"
#include "iap.h"
#include "queue.h"
#include "uni_shell.h"
#include "stm32h7xx_hal.h"

extern QueueHandle_t ota_cmd_queue;

static uint32_t packet_seq = 0;

void ota_task(void *arguments)
{
    uint16_t ota_cmd = 0;
    BaseType_t ret;
    while (1) {
        ret = xQueueReceive(ota_cmd_queue, &ota_cmd, portMAX_DELAY);
        if (ret != pdPASS) {
            LOG_WARNING("ota cmd queue receive failure");
            continue;
        }
		LOG_DEBUG("receiving ota cmd 0x%04hx from queue", ota_cmd);
        switch (ota_cmd) {
            case MSG_ID_SEND_REQUEST_KEEPALIVE: SendRequestKeepAlive(); break;
            case MSG_ID_SEND_REQUEST_NEW_FIRMWARE_INFO: SendRequestNewFirmwareInfo(); break;
            case MSG_ID_SEND_REQUEST_THE_N_PACKET: SendRequestTheNPacket(packet_seq); packet_seq++; break;
            case MSG_ID_SEND_REPORT_THE_N_PACKET_OK: SendReportNPacketOK(packet_seq); break;
            case MSG_ID_SEND_REQUEST_NOW_UTC_TIME: SendRequestNowUTCTime(); break;
            case MSG_ID_SEND_RESPONSE_OLD_FIRMWARE_INFO: SendResponseOldFirmwareInfo("test_device", VERSION_TO_UINT32(1, 1, 1), HAL_GetTick(), "None", "the version about simple ota test", 234562, 0x4AC3); break;
            default: LOG_WARNING("ota cmd: %hx not found", ota_cmd);
        }
    }
}