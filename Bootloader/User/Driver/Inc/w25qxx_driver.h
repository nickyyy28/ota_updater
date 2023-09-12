#ifndef __W25QXX_DRIVER_H
#define __W25QXX_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define USE_DEBUG 	1
#define USED_RTOS	1

#define FLASH_CS_GPIO_PORT	GPIOD
#define FLASH_CS_GPIO_PIN	GPIO_PIN_6

#define FLASH_SPI hspi1

#define W25Qxx_Read_Data_Byte 		0x03
#define W25Qxx_Write_Enable_Byte	0x06
#define W25Qxx_Write_Disable_Byte	0x04

#if USE_DEBUG == 1
#define W25Qxx_DEBUG(...) LOG_DEBUG(__VA_ARGS__)
#define W25Qxx_INFO(...) LOG_INFO(__VA_ARGS__)
#define W25Qxx_WARNING(...) LOG_WARNING(__VA_ARGS__)
#define W25Qxx_ERROR(...) LOG_ERROR(__VA_ARGS__)
#define W25Qxx_FATAL(...) LOG_FATAL(__VA_ARGS__)

#define LINE_DEBUG(fmt, ...) LOG_DEBUG("<%s:%d> " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#else
#define W25Qxx_DEBUG(...)
#define W25Qxx_INFO(...)
#define W25Qxx_WARNING(...)
#define W25Qxx_ERROR(...)
#define W25Qxx_FATAL(...)
#endif

#if USED_RTOS
#define flash_delay osDelay
#else
#define flash_delay HAL_Delay
#endif

typedef enum{
	W25Qxx_OK = 0,
	W25Qxx_Error,
	W25Qxx_Init_Fail,
}W25Qxx_Status;

typedef enum{
	W25Q80 = 1,
	W25Q16,
	W25Q32,
	W25Q64,
	W25Q128,
	W25Q256,
	W25Q512
}W25Qxx_Type;

typedef struct{
	W25Qxx_Type Type;
	uint8_t UniqID[8];
	uint32_t ID;
	uint32_t JEDEC_ID;
	uint16_t PageSize;
	uint32_t PageCount;
	uint32_t SectorSize;
	uint32_t SectorCount;
	uint32_t BlockSize;
	uint32_t BlockCount;
	uint32_t CapacityInKiloByte;
	uint8_t StatusRegister1;
	uint8_t StatusRegister2;
	uint8_t StatusRegister3;
}W25Qxx;
 
W25Qxx_Status W25Qxx_ReadID(W25Qxx *flash);
W25Qxx_Status W25Qxx_Read_JEDEC_ID(W25Qxx *flash);

W25Qxx_Status W25Qxx_Init(W25Qxx *flash);

W25Qxx_Status W25Qxx_Write_Enable();
W25Qxx_Status W25Qxx_Write_Disable();

W25Qxx_Status W25Qxx_Read_Page(uint32_t addr, uint8_t* buffer, W25Qxx *flash);

#ifdef __cplusplus
}
#endif

#endif
