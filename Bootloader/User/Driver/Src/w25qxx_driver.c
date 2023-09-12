#include "w25qxx_driver.h"

#include "spi.h"
#include "uni_shell.h"

#include "cmsis_os.h"

#define FLASH_CS_LOW() HAL_GPIO_WritePin(FLASH_CS_GPIO_PORT, FLASH_CS_GPIO_PIN, GPIO_PIN_RESET)
#define FLASH_CS_HIGH() HAL_GPIO_WritePin(FLASH_CS_GPIO_PORT, FLASH_CS_GPIO_PIN, GPIO_PIN_SET)

#define FLASH_EMPTY_Byte 0xA5

uint8_t W25Qxx_SPI_TransmitReceive(uint8_t ch)
{
	uint8_t ret;
	if (HAL_SPI_TransmitReceive(&FLASH_SPI, &ch, &ret, 1, 100) != HAL_OK) {
#if USE_DEBUG == 1
		W25Qxx_DEBUG("%s:%d HAL_SPI_TransmitReceive Error", __FILE__, __LINE__);
#endif
	}
	return ret;
}

W25Qxx_Status W25Qxx_ReadID(W25Qxx *flash)
{
	if (flash == NULL) {
		W25Qxx_ERROR("%s:%d flash is null", __FILE__, __LINE__);
		return W25Qxx_Error;
	}
	W25Qxx_Status status = W25Qxx_Init_Fail;
    FLASH_CS_LOW();

    static uint8_t read_id_cmd[6] = {0x90, 0x00, 0x00, 0x00, 0xFF, 0xFF};
    uint8_t ret[6] = {0};

	if (HAL_SPI_TransmitReceive(&FLASH_SPI, read_id_cmd, ret, 6, 100) != HAL_OK) {
		W25Qxx_ERROR("%s:%d HAL_SPI_TransmitReceive Read ID cmd receive fail", __FILE__, __LINE__);
		goto end;
	}
	
	status = W25Qxx_OK;
	W25Qxx_DEBUG("READ ID %x %x %x %x %x %x", ret[0], ret[1], ret[2], ret[3], ret[4], ret[5]);
	//flash->ID = ret[1] << 16 | ret[2] << 8 | ret[3];
	flash->ID = ret[4] << 8 | ret[5];
	W25Qxx_DEBUG("Device ID: %x", flash->ID);

#define TYPE_CASE(__id__, __type__, __size__) 	\
	case __id__: {								\
		flash->Type = __type__;					\
		flash->CapacityInKiloByte = __size__;	\
		flash->BlockCount = __size__ / (16 * 4); \
		flash->BlockSize = 16 * 4 * 1024;		\
		flash->SectorCount = __size__ / 4;		\
		flash->SectorSize = 4 * 1024;			\
		flash->PageCount = __size__ * 4;		\
		flash->PageSize = 256;					\
		break;									\
	}

	switch (flash->ID) {
		TYPE_CASE(0xEF13, W25Q80, 1 * 1024);
		TYPE_CASE(0xEF14, W25Q16, 2 * 1024);
		TYPE_CASE(0xEF15, W25Q32, 4 * 1024);
		TYPE_CASE(0xEF16, W25Q64, 8 * 1024);
		TYPE_CASE(0xEF17, W25Q128, 16 * 1024);
		TYPE_CASE(0xEF18, W25Q256, 32 * 1024);
		default:
			status = W25Qxx_Error;
			LOG_ERROR("Read W25Qxx ID %x, Not Found Type", flash->ID);
			goto end;
	}
	
	W25Qxx_DEBUG("Device Capacity: %d KB", flash->CapacityInKiloByte);
	W25Qxx_DEBUG("Device Block Count %d", flash->BlockCount);
	W25Qxx_DEBUG("Device Block Size %d Bytes", flash->BlockSize);
	W25Qxx_DEBUG("Device Sector Count %d", flash->SectorCount);
	W25Qxx_DEBUG("Device Sector Size %d Bytes", flash->SectorSize);
	W25Qxx_DEBUG("Device Page Count %d", flash->PageCount);
	W25Qxx_DEBUG("Device Page Size %d Bytes", flash->PageSize);
#undef TYPE_CASE
	
end:
    FLASH_CS_HIGH();
	return status;
}

W25Qxx_Status W25Qxx_Read_JEDEC_ID(W25Qxx *flash) 
{
	if (flash == NULL) {
		W25Qxx_ERROR("%s:%d flash is null", __FILE__, __LINE__);
		return W25Qxx_Error;
	}
	W25Qxx_Status status = W25Qxx_Init_Fail;
    FLASH_CS_LOW();

    static uint8_t read_id_cmd[4] = {0x9F, FLASH_EMPTY_Byte, FLASH_EMPTY_Byte, FLASH_EMPTY_Byte};
    uint8_t ret[4] = {0};

	if (HAL_SPI_TransmitReceive(&FLASH_SPI, read_id_cmd, ret, 4, 100) != HAL_OK) {
		W25Qxx_ERROR("%s:%d HAL_SPI_TransmitReceive Read JEDEC ID cmd receive fail", __FILE__, __LINE__);
		goto end;
	}
	
	status = W25Qxx_OK;
	W25Qxx_DEBUG("READ JEDEC ID %x %x %x %x", ret[0], ret[1], ret[2], ret[3]);
	flash->JEDEC_ID = ret[1] << 16 | ret[2] << 8 | ret[3];

#define TYPE_CASE(__id__, __type__, __size__) 	\
	case __id__: {								\
		flash->Type = __type__;					\
		flash->CapacityInKiloByte = __size__;	\
		flash->BlockCount = __size__ / (16 * 4); \
		flash->BlockSize = 16 * 4 * 1024;		\
		flash->SectorCount = __size__ / 4;		\
		flash->SectorSize = 4 * 1024;			\
		flash->PageCount = __size__ * 4;		\
		flash->PageSize = 256;					\
		break;									\
	}

	switch (flash->JEDEC_ID) {
		TYPE_CASE(0xEF4014, W25Q80, 1 * 1024);
		TYPE_CASE(0xEF4015, W25Q16, 2 * 1024);
		TYPE_CASE(0xEF4016, W25Q32, 4 * 1024);
		TYPE_CASE(0xEF4017, W25Q64, 8 * 1024);
		TYPE_CASE(0xEF4018, W25Q128, 16 * 1024);
		TYPE_CASE(0xEF4019, W25Q256, 32 * 1024);
		default:
			status = W25Qxx_Error;
			LOG_ERROR("Read W25Qxx JEDEC ID %x, Not Found Type", flash->JEDEC_ID);
			goto end;
	}

	W25Qxx_DEBUG("Device Capacity: %d KB", flash->CapacityInKiloByte);
	W25Qxx_DEBUG("Device Block Count %d", flash->BlockCount);
	W25Qxx_DEBUG("Device Block Size %d Bytes", flash->BlockSize);
	W25Qxx_DEBUG("Device Sector Count %d", flash->SectorCount);
	W25Qxx_DEBUG("Device Sector Size %d Bytes", flash->SectorSize);
	W25Qxx_DEBUG("Device Page Count %d", flash->PageCount);
	W25Qxx_DEBUG("Device Page Size %d Bytes", flash->PageSize);

#undef TYPE_CASE

end:
    FLASH_CS_HIGH();
	return status;
}

W25Qxx_Status W25Qxx_Init(W25Qxx *flash)
{
	if (flash == NULL) {
		W25Qxx_ERROR("%s:%d flash is null", __FILE__, __LINE__);
		return W25Qxx_Error;
	}
	
	return W25Qxx_ReadID(flash);
}

W25Qxx_Status W25Qxx_Write_Enable()
{
	return W25Qxx_Error;
}

W25Qxx_Status W25Qxx_Write_Disable()
{
	return W25Qxx_Error;
}

W25Qxx_Status W25Qxx_Read_Page(uint32_t addr, uint8_t* buffer, W25Qxx *flash)
{
	if (flash == NULL) {
		W25Qxx_ERROR("%s:%d flash is null", __FILE__, __LINE__);
		return W25Qxx_Error;
	}

	W25Qxx_Status ret = W25Qxx_Error;

	FLASH_CS_LOW();

	static char read_cmd[5] = {0};

	read_cmd[0] = W25Qxx_Read_Data_Byte;
	uint32_t len;
	if (flash->Type >= W25Q256) {
		read_cmd[1] = (uint8_t)(addr >> 24);
		read_cmd[2] = (uint8_t)(addr >> 16);
		read_cmd[3] = (uint8_t)(addr >> 8);
		read_cmd[4] = (uint8_t)(addr);
		len = 5;
	} else {
		read_cmd[1] = (uint8_t)(addr >> 16);
		read_cmd[2] = (uint8_t)(addr >> 8);
		read_cmd[3] = (uint8_t)(addr);
		len = 4;
	}

	if (HAL_SPI_Transmit(&FLASH_SPI, read_cmd, len, 100) != HAL_OK) {
		W25Qxx_ERROR("Send Read Data Cmd Fail");
		ret = W25Qxx_Error;
		goto end;
	}

	if (HAL_SPI_Receive(&FLASH_SPI, buffer, flash->PageSize, 100) != HAL_OK) {
		W25Qxx_ERROR("Read Page Data Error");
		ret = W25Qxx_Error;
		goto end;
	}
	ret = W25Qxx_OK;
end:

	FLASH_CS_HIGH();

	return ret;
}
