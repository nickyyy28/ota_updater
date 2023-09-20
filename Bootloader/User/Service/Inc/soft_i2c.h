#ifndef __SOFT_IIC_H
#define __SOFT_IIC_H

#include <stdint.h>

#define IIC_SCL_H SCL_Set_Pin(1)
#define IIC_SCL_L SCL_Set_Pin(0)
#define IIC_SDA_H SDA_Set_Pin(1)
#define IIC_SDA_L SDA_Set_Pin(0)

void IIC_Start(void);
void IIC_Stop(void);
void IIC_Init(void);
uint8_t IIC_Wait_ACK(void);
void IIC_ACK(void);
void IIC_NACK(void);
uint8_t IIC_Send_Byte(uint8_t ch);
uint8_t IIC_Recv_Byte(uint8_t ack);

uint8_t IIC_Read_Mem(uint8_t device_addr, uint16_t mem_addr, uint8_t *buffer, uint32_t len);
uint8_t IIC_Write_Mem(uint8_t device_addr, uint16_t mem_addr, const uint8_t *buffer, uint32_t len);

#endif