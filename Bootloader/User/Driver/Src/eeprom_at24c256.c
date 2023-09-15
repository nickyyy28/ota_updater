#include "eeprom_at24c256.h"
#include "soft_i2c.h"

void eeprom_read_page(uint8_t device_addr, uint16_t page_addr, uint8_t in_page_addr, uint8_t *buffer, uint32_t len)
{
	uint8_t device_read = device_addr | 1;
	
	uint16_t read_addr = (uint16_t)in_page_addr | (page_addr << 6);
	
	if (page_addr >= 512) {
		LOG_ERROR("PAGE ADDR NEED LESS THAN 512, NOW IS %d", page_addr);
		goto end;
	}
	
	if (len + in_page_addr > 64) {
		LOG_ERROR("PAGE SIZE 64 BYTE, NOW LEN IS %d, ADDR IN PAGE IS %d, OVER MAX ADDR", len, in_page_addr);
		goto end;
	}
	
	IIC_Start();
	IIC_Send_Byte(device_read);
	if (IIC_Wait_ACK()) {
		LOG_ERROR("Send Device Addr Wait ACK Fail");
		goto end;
	}
	IIC_Send_Byte((uint8_t)read_addr);
	if (IIC_Wait_ACK()) {
		LOG_ERROR("Send Read High Addr Wait ACK Fail");
		goto end;
	}
	IIC_Send_Byte((uint8_t)read_addr >> 8);
	if (IIC_Wait_ACK()) {
		LOG_ERROR("Send Read Low Addr Wait ACK Fail");
		goto end;
	}
	uint32_t i = 0;
	for (i = 0 ; i < len - 1 ; i++) {
		buffer[i] = IIC_Recv_Byte(1);
	}
	
	buffer[i] = IIC_Recv_Byte(0);
	
end:
	IIC_Stop();
}

void eeprom_write_str(uint8_t device_addr, uint8_t write_addr, uint8_t *buffer, uint32_t len)
{
	uint8_t device_write = device_addr;
}
