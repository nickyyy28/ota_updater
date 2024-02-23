#include "bsp_fdcan.h"

void FDCAN_Filter_Init(void)
{
	FDCAN_FilterTypeDef filter;
	filter.IdType = FDCAN_STANDARD_ID;
	filter.FilterIndex = 0;
	filter.FilterType = FDCAN_FILTER_MASK;
	filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	filter.FilterID1 = 0x0000;
	filter.FilterID2 = 0x0000;
	if (HAL_FDCAN_ConfigFilter(&hfdcan1, &filter) != HAL_OK) {
		Error_Handler();
	}
	
	HAL_FDCAN_Start(&hfdcan1);
	HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
	
}

uint8_t FDCAN_receive_msg(FDCAN_HandleTypeDef *hfdcan, FDCAN_RxHeaderTypeDef *rxheader, uint8_t *buffer)
{
	if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, rxheader, buffer) != HAL_OK) {
		//receive msg fail
		return 0;
	} else {
		return 1;
	}
}

uint8_t FDCAN_transmit_msg(FDCAN_HandleTypeDef *hfdcan, FDCAN_TxHeaderTypeDef *txheader, uint8_t *buffer)
{
	if (HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, txheader, buffer) != HAL_OK) {
		return 0;
	} else {
		return 1;
	}
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET) {
		if (hfdcan->Instance == FDCAN1) {
			asm("nop");
		}
	}
}