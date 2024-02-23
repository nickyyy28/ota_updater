#ifndef __BSP_FDCAN_H
#define __BSP_FDCAN_H

#include <stdint.h>
#include "fdcan.h"

typedef enum{
	RECEIVE_OK = 0,
	RECEIVE_NOK
}FDCAN_RECV_RESLUT; 

void FDCAN_Filter_Init(void);

uint8_t FDCAN_receive_msg(FDCAN_HandleTypeDef *hfdcan, FDCAN_RxHeaderTypeDef *rxheader, uint8_t *buffer);
uint8_t FDCAN_transmit_msg(FDCAN_HandleTypeDef *hfdcan, FDCAN_TxHeaderTypeDef *txheader, uint8_t *buffer);

#endif
