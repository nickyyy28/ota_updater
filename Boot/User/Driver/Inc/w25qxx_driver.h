#ifndef __W25QXX_DRIVER_H
#define __W25QXX_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "log.h"

#define USE_DEBUG 	1
#define USED_RTOS	0

#define __LITTLE_ENDIAN_ 	0
#define __BIG_ENDIAN_		1

#define BYTE_ORDER 	__LITTLE_ENDIAN_

#define FLASH_CS_GPIO_PORT	GPIOD
#define FLASH_CS_GPIO_PIN	GPIO_PIN_6

#define FLASH_SPI hspi1

#define W25QXX_WRITE_DATA_CMD		0x02
#define W25QXX_READ_DATA_CMD 		0x03
#define W25QXX_WRITE_ENABLE_CMD		0x06
#define W25QXX_WRITE_DISABLE_CMD	0x04

#define W25QXX_READ_STATUS_REGISTER1_CMD	0x05
#define W25QXX_READ_STATUS_REGISTER2_CMD	0x35
#define W25QXX_READ_STATUS_REGISTER3_CMD	0x15

#define W25QXX_WRITE_STATUS_REGISTER1_CMD	0x01
#define W25QXX_WRITE_STATUS_REGISTER2_CMD	0x31
#define W25QXX_WRITE_STATUS_REGISTER3_CMD	0x11

#define W25QXX_ERASE_SECTOR_CMD		0x20

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
	W25Qxx_Timeout,
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

#if BYTE_ORDER == __BIG_ENDIAN_
	struct
	{
		uint8_t SRP : 1;
		uint8_t SEC : 1;
		uint8_t TB : 1;
		uint8_t BP2 : 1;
		uint8_t BP1 : 1;
		uint8_t BP0 : 1;
		uint8_t WEL : 1;
		uint8_t BUSY : 1;
	}StatusRegister1;

#elif BYTE_ORDER == __LITTLE_ENDIAN_
	union
	{
		uint8_t buffer;
		struct {
			uint8_t BUSY : 1;
			uint8_t WEL : 1;
			uint8_t BP0 : 1;
			uint8_t BP1 : 1;
			uint8_t BP2 : 1;
			uint8_t TB : 1;
			uint8_t SEC : 1;
			uint8_t SRP : 1;
		}reg;
	}StatusRegister1;

	union
	{
		uint8_t buffer;
		struct {
			uint8_t SRL : 1;
			uint8_t QE : 1;
			uint8_t  : 1;
			uint8_t LB1 : 1;
			uint8_t LB2 : 1;
			uint8_t LB3 : 1;
			uint8_t CMP : 1;
			uint8_t SUS : 1;
		}reg;
	}StatusRegister2;

	union
	{
		uint8_t buffer;
		struct {
			uint8_t  : 2;
			uint8_t WPS : 1;
			uint8_t  : 2;
			uint8_t DRV0 : 1;
			uint8_t DRV1 : 1;
			uint8_t  : 1;
		}reg;
	}StatusRegister3;
#endif
} W25Qxx;

/**
 * @brief read device id
 * 
 * @param flash 
 * @return W25Qxx_Status 
 */
W25Qxx_Status w25qxx_read_id(W25Qxx *flash);

/**
 * @brief read jedec id
 * 
 * @param flash 
 * @return W25Qxx_Status 
 */
W25Qxx_Status w25qxx_read_JEDEC_id(W25Qxx *flash);

/**
 * @brief w25qxx init
 * 
 * @param flash 
 * @return W25Qxx_Status 
 */
W25Qxx_Status w25qxx_init(W25Qxx *flash);

/**
 * @brief w25qxx write enable
 * 
 * @return W25Qxx_Status 
 */
W25Qxx_Status w25qxx_write_enable(void);

/**
 * @brief w5qxx write disable
 * 
 * @return W25Qxx_Status 
 */
W25Qxx_Status w25qxx_write_disable(void);

/**
 * @brief read w25qxx status register
 * 
 * @param flash 
 * @param id 		num 1-3
 * @return W25Qxx_Status 
 */
W25Qxx_Status w25qxx_read_status_register(W25Qxx *flash, uint8_t id);

/**
 * @brief read w25qxx page data
 * 
 * @param addr page addr. 0-pageCount
 * @param buffer 
 * @param flash 
 * @return W25Qxx_Status 
 */
W25Qxx_Status w25qxx_read_page(W25Qxx *flash, uint32_t page_addr, uint8_t* buffer, uint32_t len);

/**
 * @brief write w25qxx data on page
 * 
 * @param flash 
 * @param page_addr 
 * @param buffer 
 * @return W25Qxx_Status 
 */
W25Qxx_Status w25qxx_write_page(W25Qxx *flash, uint32_t page_addr, const uint8_t* buffer, uint32_t len);

/**
 * @brief read w25qxx sector data
 * 
 * @param flash 
 * @param sector_addr 
 * @param buffer 
 * @param len 
 * @return W25Qxx_Status 
 */
W25Qxx_Status w25qxx_read_sector(W25Qxx *flash, uint32_t sector_addr, uint8_t *buffer, uint32_t len);

/**
 * @brief write w25qxx data on sector
 * 
 * @param flash 
 * @param sector_addr 
 * @param buffer 
 * @param len 
 * @return W25Qxx_Status 
 */
W25Qxx_Status w25qxx_write_sector(W25Qxx *flash, uint32_t sector_addr, const uint8_t* buffer, uint32_t len);

/**
 * @brief erase w25qxx sector
 * 
 * @param flash 
 * @param sector_addr sector addr, 0-sectorCount
 * @return W25Qxx_Status 
 */
W25Qxx_Status w25qxx_erase_sector(W25Qxx *flash, uint32_t sector_addr);

/**
 * @brief wait w25qxx out of busy
 * 
 * @param flash
 * @param timeout timeout
 * @return W25Qxx_Status 
 */
W25Qxx_Status w25qxx_wait_busy(W25Qxx *flash, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif
