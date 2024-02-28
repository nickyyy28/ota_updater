#include "w25qxx_driver.h"

#include "spi.h"

#if USE_DEBUG

#include "uni_shell.h"
#include "cmsis_os.h"

#endif



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

W25Qxx_Status w25qxx_read_id(W25Qxx *flash)
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
			W25Qxx_ERROR("Read W25Qxx ID %x, Not Found Type", flash->ID);
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

W25Qxx_Status w25qxx_read_JEDEC_id(W25Qxx *flash) 
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
			W25Qxx_ERROR("Read W25Qxx JEDEC ID %x, Not Found Type", flash->JEDEC_ID);
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

W25Qxx_Status w25qxx_init(W25Qxx *flash)
{
	if (flash == NULL) {
		W25Qxx_ERROR("%s:%d flash is null", __FILE__, __LINE__);
		return W25Qxx_Error;
	}
	
	return w25qxx_read_id(flash);
}

W25Qxx_Status w25qxx_write_enable()
{
	FLASH_CS_LOW();
	W25Qxx_Status ret = W25Qxx_Error;
	uint8_t cmd = W25QXX_WRITE_ENABLE_CMD;
	if (HAL_SPI_Transmit(&FLASH_SPI, &cmd, 1, 100)) {
		ret = W25Qxx_Error;
		goto end;
	}
	ret = W25Qxx_OK;
end:
	FLASH_CS_HIGH();
	return ret;
}

W25Qxx_Status w25qxx_write_disable()
{
	FLASH_CS_LOW();
	W25Qxx_Status ret = W25Qxx_Error;
	uint8_t cmd = W25QXX_WRITE_DISABLE_CMD;
	if (HAL_SPI_Transmit(&FLASH_SPI, &cmd, 1, 100)) {
		ret = W25Qxx_Error;
		goto end;
	}
	ret = W25Qxx_OK;
end:
	FLASH_CS_HIGH();
	return ret;
}

W25Qxx_Status w25qxx_read_page(W25Qxx *flash, uint32_t page_addr, uint8_t* buffer, uint32_t len)
{
	if (flash == NULL) {
		W25Qxx_ERROR("%s:%d flash is null", __FILE__, __LINE__);
		return W25Qxx_Error;
	}
	
	uint32_t start_time = HAL_GetTick();

	W25Qxx_Status ret = W25Qxx_Error;
	page_addr *= flash->PageSize;
	if (len > flash->PageSize) {
		W25Qxx_ERROR("read data length %d big than max size: %d", len, flash->PageSize);
		goto end;
	}

	FLASH_CS_LOW();

	static char read_cmd[5] = {0};

	read_cmd[0] = W25QXX_READ_DATA_CMD;
	uint32_t cmd_len;
	if (flash->Type >= W25Q256) {
		read_cmd[1] = (uint8_t)(page_addr >> 24);
		read_cmd[2] = (uint8_t)(page_addr >> 16);
		read_cmd[3] = (uint8_t)(page_addr >> 8);
		read_cmd[4] = (uint8_t)(page_addr);
		cmd_len = 5;
	} else {
		read_cmd[1] = (uint8_t)(page_addr >> 16);
		read_cmd[2] = (uint8_t)(page_addr >> 8);
		read_cmd[3] = (uint8_t)(page_addr);
		cmd_len = 4;
	}

	if (HAL_SPI_Transmit(&FLASH_SPI, (const uint8_t*)read_cmd, cmd_len, 100) != HAL_OK) {
		W25Qxx_ERROR("Send Read Data Cmd Fail");
		ret = W25Qxx_Error;
		goto end;
	}

	/*if (HAL_SPI_Receive(&FLASH_SPI, buffer, len, 1000) != HAL_OK) {
		W25Qxx_ERROR("Read Page Data Error");
		ret = W25Qxx_Error;
		goto end;
	}*/
	
	for (uint32_t i = 0 ; i < len ; i++) {
		uint8_t ret = W25Qxx_SPI_TransmitReceive(0xFF);
		buffer[i] = ret;
	}
	
	ret = W25Qxx_OK;
end:

	FLASH_CS_HIGH();
	start_time = HAL_GetTick() - start_time;
	W25Qxx_DEBUG("read page: %d total %d bytes cost %d ms", page_addr, len, start_time);

	return ret;
}

W25Qxx_Status w25qxx_read_sector(W25Qxx *flash, uint32_t sector_addr, uint8_t *buffer, uint32_t len)
{
	if (flash == NULL) {
		W25Qxx_ERROR("%s:%d flash is null", __FILE__, __LINE__);
		return W25Qxx_Error;
	}

	W25Qxx_Status ret = W25Qxx_Error;
	uint32_t base_page_addr = sector_addr * 16;
	sector_addr *= flash->SectorSize;
	if (len > flash->SectorSize) {
		W25Qxx_ERROR("read data length %d big than max size: %d", len, flash->SectorSize);
		goto end;
	}

	for (uint32_t index = 0 ; index < len ; index += flash->PageSize) {
		if (w25qxx_read_page(flash, base_page_addr, buffer + index, ((len - index > 256) ? 256 : (len - index)))) {
			W25Qxx_ERROR("read page %d occured error", base_page_addr);
			goto end;
		}
		base_page_addr ++;
	}

	ret = W25Qxx_OK;
end:
	return ret;
}

W25Qxx_Status w25qxx_write_page(W25Qxx *flash, uint32_t page_addr, const uint8_t* buffer, uint32_t len)
{
	if (flash == NULL) {
		W25Qxx_ERROR("%s:%d flash is null", __FILE__, __LINE__);
		return W25Qxx_Error;
	}

	W25Qxx_Status ret = W25Qxx_Error;
	page_addr *= flash->PageSize;
	if (len > flash->PageSize) {
		W25Qxx_ERROR("write data length %d big than max size: %d", len, flash->PageSize);
		goto end;
	}
	
	if (w25qxx_write_enable()) {
		W25Qxx_ERROR("send write enable signal fail");
		goto end;
	}

	FLASH_CS_LOW();

	static char write_cmd[5] = {0};

	write_cmd[0] = W25QXX_WRITE_DATA_CMD;
	uint32_t cmd_len;
	if (flash->Type >= W25Q256) {
		write_cmd[1] = (uint8_t)(page_addr >> 24);
		write_cmd[2] = (uint8_t)(page_addr >> 16);
		write_cmd[3] = (uint8_t)(page_addr >> 8);
		write_cmd[4] = (uint8_t)(page_addr);
		cmd_len = 5;
	} else {
		write_cmd[1] = (uint8_t)(page_addr >> 16);
		write_cmd[2] = (uint8_t)(page_addr >> 8);
		write_cmd[3] = (uint8_t)(page_addr);
		cmd_len = 4;
	}

	if (HAL_SPI_Transmit(&FLASH_SPI, (const uint8_t*)write_cmd, cmd_len, 1000) != HAL_OK) {
		W25Qxx_ERROR("send write data cmd fail");
		ret = W25Qxx_Error;
		goto end;
	} 

	if (HAL_SPI_Transmit(&FLASH_SPI, buffer, len, 1000)) {
		W25Qxx_ERROR("write page data error");
		ret = W25Qxx_Error;
		goto end;
	}

	ret = W25Qxx_OK;

end:

	FLASH_CS_HIGH();
	w25qxx_wait_busy(flash, 0);
	return ret;
}

W25Qxx_Status w25qxx_write_sector(W25Qxx *flash, uint32_t sector_addr, const uint8_t* buffer, uint32_t len)
{
	if (flash == NULL) {
		W25Qxx_ERROR("%s:%d flash is null", __FILE__, __LINE__);
		return W25Qxx_Error;
	}

	W25Qxx_Status ret = W25Qxx_Error;
	uint32_t base_page_addr = sector_addr * 16;
	sector_addr *= flash->SectorSize;
	if (len > flash->SectorSize) {
		W25Qxx_ERROR("write data length %d big than max size: %d", len, flash->SectorSize);
		goto end;
	}

	for (uint32_t index = 0 ; index < len ; index += flash->PageSize) {
		if (w25qxx_write_page(flash, base_page_addr, buffer + index, ((len - index > 256) ? 256 : (len - index)))) {
			W25Qxx_ERROR("write page %d occured error", base_page_addr);
			goto end;
		}
		base_page_addr ++;
	}

	ret = W25Qxx_OK;
end:
	return ret;
}

W25Qxx_Status w25qxx_erase_sector(W25Qxx *flash, uint32_t sector_addr)
{
	if (flash == NULL) {
		W25Qxx_ERROR("%s:%d flash is null", __FILE__, __LINE__);
		return W25Qxx_Error;
	}
	
	uint32_t start_tick = HAL_GetTick();

	w25qxx_wait_busy(flash, 0);
	w25qxx_write_enable();

	W25Qxx_Status ret = W25Qxx_Error;
	sector_addr *= flash->SectorSize;

	FLASH_CS_LOW();

	static char read_cmd[5] = {0};

	read_cmd[0] = W25QXX_ERASE_SECTOR_CMD;
	uint32_t len;
	if (flash->Type >= W25Q256) {
		read_cmd[1] = (uint8_t)(sector_addr >> 24);
		read_cmd[2] = (uint8_t)(sector_addr >> 16);
		read_cmd[3] = (uint8_t)(sector_addr >> 8);
		read_cmd[4] = (uint8_t)(sector_addr);
		len = 5;
	} else {
		read_cmd[1] = (uint8_t)(sector_addr >> 16);
		read_cmd[2] = (uint8_t)(sector_addr >> 8);
		read_cmd[3] = (uint8_t)(sector_addr);
		len = 4;
	}

	if (HAL_SPI_Transmit(&FLASH_SPI, (const uint8_t*)read_cmd, len, 1000) != HAL_OK) {
		W25Qxx_ERROR("send erase cmd fail");
		ret = W25Qxx_Error;
		goto end;
	}
	ret = W25Qxx_OK;
end:
	W25Qxx_DEBUG("erase sector use %d ms", HAL_GetTick() - start_tick);
	FLASH_CS_HIGH();
	w25qxx_wait_busy(flash, 0);
	return ret;
}

W25Qxx_Status w25qxx_read_status_register(W25Qxx *flash, uint8_t id)
{
	W25Qxx_Status ret = W25Qxx_Error;

	FLASH_CS_LOW();
	uint8_t cmd = 0;
	uint8_t *reg = NULL;
	switch (id) {
		case 1:
			cmd = W25QXX_READ_STATUS_REGISTER1_CMD;
			reg = &flash->StatusRegister1.buffer;
			break;
		case 2:
			cmd = W25QXX_READ_STATUS_REGISTER2_CMD;
			reg = &flash->StatusRegister2.buffer;
			break;
		case 3:
			cmd = W25QXX_READ_STATUS_REGISTER3_CMD;
			reg = &flash->StatusRegister3.buffer;
			break;
		default:
			goto end;
	}

	if (HAL_SPI_Transmit(&FLASH_SPI, &cmd, 1, 1000) != HAL_OK) {
		W25Qxx_ERROR("send read status register %d command fail", cmd);
		ret = W25Qxx_Error;
		goto end;
	}


	if (HAL_SPI_Receive(&FLASH_SPI, reg, 1, 1000) != HAL_OK) {
		W25Qxx_ERROR("receive register data %d fail", cmd);
		ret = W25Qxx_Error;
		goto end;
	}

end:
	FLASH_CS_HIGH();
	return ret;
}

W25Qxx_Status w25qxx_wait_busy(W25Qxx *flash, uint32_t timeout)
{
	if (timeout == 0) {
		do {
			w25qxx_read_status_register(flash, 1);
			flash_delay(1);
		} while (flash->StatusRegister1.reg.BUSY);
		return W25Qxx_OK;
	} else {
		while (timeout--) {
			w25qxx_read_status_register(flash, 1);
			if (!flash->StatusRegister1.reg.BUSY) {
				return W25Qxx_OK;
			}
			flash_delay(1);
		}
		return W25Qxx_Timeout;
	}
}