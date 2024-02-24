#include "comTask.h"

#include "cmsis_os.h"
#include "bsp_fdcan.h"

#include <string.h>
#include "uni_shell.h"

#include "usart.h"

FDCAN_TxHeaderTypeDef txheader;
uint8_t tx_buffer[8] = {1, 2, 3, 4, 5, 6, 7, 8};
uint8_t tx_reslut = 0;

FDCAN_RxHeaderTypeDef rxheader;
uint8_t rx_buffer[8] = {0};
uint8_t rx_reslut = 0;


void comTask(void *arg)
{
	txheader.Identifier = 0x201;
	txheader.IdType = FDCAN_STANDARD_ID;
	txheader.TxFrameType = FDCAN_DATA_FRAME;
	txheader.DataLength = FDCAN_DLC_BYTES_8;
	txheader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	txheader.BitRateSwitch = FDCAN_BRS_OFF;
	txheader.FDFormat = FDCAN_CLASSIC_CAN;
	txheader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	txheader.MessageMarker = 0;
	
	while(1) {
		
		rx_reslut = FDCAN_receive_msg(&hfdcan1, &rxheader, rx_buffer);
		
		if (rx_reslut) {
			LOG_INFO("Received Data");
			memcpy(tx_buffer, rx_buffer, sizeof(rx_buffer));
			tx_reslut = FDCAN_transmit_msg(&hfdcan1, &txheader, tx_buffer);
		}
#ifdef USE_APP_AERA1
		HAL_UART_Transmit(&huart1, "Into APP1\r\n", 11, 10);
#elif defined USE_APP_AERA2
		HAL_UART_Transmit(&huart1, "Into APP2\r\n", 11, 10);
#endif
		osDelay(100);
	}
}