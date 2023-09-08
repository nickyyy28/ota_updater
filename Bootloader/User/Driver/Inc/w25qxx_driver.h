#ifndef __W25QXX_DRIVER_H
#define __W25QXX_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define USE_DEBUG 	1
#define USE_RTOS	0

#define FLASH_CS_GPIO_PORT	GPIOD
#define FLASH_CS_GPIO_PIN	GPIO_PIN_7

#define FLASH_SPI hspi1

typedef enum{
	W25Qxx_OK = 0,
	W25Qxx_Error,
}W25Qxx_Status;

typedef enum{
	W25Q10 = 1,
	W25Q20,
	W25Q40,
	W25Q80,
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


#ifdef __cplusplus
}
#endif

#endif