#include "w25qxx_driver.h"

#include "spi.h"
#include "uni_shell.h"

#define FLASH_CS_LOW() HAL_GPIO_WritePin(FLASH_CS_GPIO_PORT, FLASH_CS_GPIO_PIN, GPIO_PIN_RESET)
#define FLASH_CS_HIGH() HAL_GPIO_WritePin(FLASH_CS_GPIO_PORT, FLASH_CS_GPIO_PIN, GPIO_PIN_SET)

#define FLASH_EMPTY_Byte 0xA5

uint8_t W25Qxx_SPI_TransmitReceive(uint8_t ch)
{
	uint8_t ret;
	if (HAL_SPI_TransmitReceive(&FLASH_SPI, &ch, &ret, 1, 100) != HAL_OK) {
#if USE_DEBUG == 1
		SHELL_DEBUG("%s:%d HAL_SPI_TransmitReceive Error", __FILE__, __LINE__);
#endif
	}
	return ret;
}

W25Qxx_Status W25Qxx_ReadID(W25Qxx *flash)
{
    FLASH_CS_LOW();

    static uint8_t read_id_cmd[4] = {0x9F, FLASH_EMPTY_Byte, FLASH_EMPTY_Byte, FLASH_EMPTY_Byte};
    uint8_t ret[4] = {0};

    W25Qxx_SPI_TransmitReceive(0x9F);
	ret[1] = W25Qxx_SPI_TransmitReceive(0xA5);
	ret[2] = W25Qxx_SPI_TransmitReceive(0xA5);
	ret[3] = W25Qxx_SPI_TransmitReceive(0xA5);
	
	SHELL_DEBUG("%x %x %x %x", ret[0], ret[1], ret[2], ret[3]);	

    FLASH_CS_HIGH();
}

