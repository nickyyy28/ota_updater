#include "shellTask.h"

#include "main.h"
#include "usbd_cdc_if.h"
#include "uni_shell.h"
#include "usb_device.h"
#include <string.h>
#include "usart.h"
#include "w25qxx_driver.h"
#include "soft_i2c.h"
#include "eeprom_at24c256.h"
#include "oled_ssd1306_1_3.h"

W25Qxx flash1 = {0};

extern void FileTest(void);

void show_msg(Command_t *cmd)
{
	int var1 = 0;
	float var2 = 0.0f;
	command_get_param_value(cmd, "fvar", &var2);
	command_get_param_value(cmd, "ivar", &var1);

	LOG_INFO("show ivar=%d, fvar=%f", var1, var2);
	osDelay(1);
}

void cmd_setled(Command_t *cmd)
{
	uint8_t led = 0;
	command_get_param_value(cmd, "led1", &led);
	if (led) {
		//HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
		LOG_INFO("LED1 open");
		osDelay(1);
	} else {
		//HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
		LOG_INFO("LED1 close");
		osDelay(1);
	}
}
/*
void cmd_spiflash(Command_t *cmd)
{
	if (param_is_configured(cmd, "readid")) {
		SHELL_LOG("readid configured");
	}
	if (param_is_configured(cmd, "read")) {
		SHELL_LOG("read configured");
	}
	if (param_is_configured(cmd, "write")) {
		SHELL_LOG("write configured");
	}
	if (param_is_configured(cmd, "addr")) {
		SHELL_LOG("addr configured");
	}
	if (param_is_configured(cmd, "value")) {
		SHELL_LOG("value configured");
	}
	
	if (param_is_configured(cmd, "readid")) {
		uint32_t id = W25qxx_ReadID();
#define FLASH_ID_CASES(__id__, __type__) \
		case __id__:						\
			SHELL_LOG("flash id is:0x%02x, type:%s", __id__, __type__);	\
			break;
		
		switch (id & 0x000000FF)
		{
			FLASH_ID_CASES(0x20, "W25Q512");
			FLASH_ID_CASES(0x19, "W25Q256");
			FLASH_ID_CASES(0x18, "W25Q128");
			FLASH_ID_CASES(0x17, "W25Q64");
			FLASH_ID_CASES(0x16, "W25Q32");
			FLASH_ID_CASES(0x15, "W25Q16");
			FLASH_ID_CASES(0x14, "W25Q80");
			FLASH_ID_CASES(0x13, "W25Q40");
			FLASH_ID_CASES(0x12, "W25Q20");
			FLASH_ID_CASES(0x11, "W25Q10");
			default:
				SHELL_LOG("flash can't read chip id");
				osDelay(1);
				break;
		}
	} else if (param_is_configured(cmd, "read") && param_is_configured(cmd, "addr")) {
		uint32_t addr;
		command_get_param_value(cmd, "addr", &addr);
		uint8_t buffer = 0;
		W25qxx_ReadByte(&buffer, addr);
		SHELL_LOG("read 0x%x value:%02x", addr, buffer);
	} else if (param_is_configured(cmd, "write") && param_is_configured(cmd, "addr") && param_is_configured(cmd, "value")) {
		uint32_t addr = 0, value = 0;
		command_get_param_value(cmd, "addr", &addr);
		command_get_param_value(cmd, "value", &value);
		W25qxx_WriteByte(value, addr);
	} else {
		SHELL_LOG("你写的什么勾八指令,正确用法:");
		SHELL_LOG("spiflash -read -addr 0x00000000");
		SHELL_LOG("spiflash -write -addr 0x00000000 -value 0x20");
	}
}*/

void cmd_spi(Command_t *cmd)
{
	uint32_t addr = 0, value = 0;
	if (param_is_configured(cmd, "read")) {
		if (param_is_configured(cmd, "addr")) {
			command_get_param_value(cmd, "addr", &addr);
			
		}
	} else if (param_is_configured(cmd, "write")) {
		if (param_is_configured(cmd, "addr") && param_is_configured(cmd, "value")) {
			command_get_param_value(cmd, "addr", &addr);
			command_get_param_value(cmd, "value", &value);
		}
	}

}

#include "w25qxx_driver.h"

uint8_t eeprom_str[64] = {0};

void cmd_filetest(Command_t *cmd)
{
	//FileTest();
	/*W25Qxx_ReadID(&flash1);
	LOG_DEBUG("-------------------------");
	W25Qxx_Read_JEDEC_ID(&flash1);
	RCC_ClkInitTypeDef clkConfig;
	uint32_t frq;
	HAL_RCC_GetClockConfig(&clkConfig, FLASH_LATENCY_0);
	frq = HAL_RCC_GetSysClockFreq();
	LOG_DEBUG("CPU Frq %d Hz", frq);*/
	
	//eeprom_read_page(0xA0, 0, 0, eeprom_str, 10);
	//LOG_INFO("read str %s", eeprom_str);
	
	extern void IIC_Test();
	//IIC_Test();
	OLED_Init();
	LOG_INFO("OLED Init...");
	osDelay(1000);
	LOG_INFO("OLED Clear");
	OLED_Clear();
	osDelay(1000);
	OLED_Show_Char(1, 1, 'A');
	LOG_INFO("OLED Show Char");
}

void shell_task(void* param)
{
	shell_init();
	IIC_Init();
	osDelay(100);
	OLED_Init();
	//W25qxx_Init();
	//MX_USB_DEVICE_Init();
	Command_t *cmd = register_command("show", show_msg);
	if (cmd != NULL) {
		command_add_param(cmd, "fvar", TYPE_FLOAT);
		command_add_param(cmd, "ivar", TYPE_INT);
	}
	
	cmd = register_command("setled", cmd_setled);
	if (cmd != NULL) {
		command_add_param(cmd, "led1", TYPE_BOOL);
	}
	
	/*cmd = register_command("spiflash", cmd_spiflash);
	if (cmd != NULL) {
		command_add_param(cmd, "readid", TYPE_BOOL);
		command_add_param(cmd, "read", TYPE_BOOL);
		command_add_param(cmd, "write", TYPE_BOOL);
		command_add_param(cmd, "addr", TYPE_UINT32HEX);
		command_add_param(cmd, "value", TYPE_UINT32HEX);
	}*/
	
	cmd = register_command("spi", cmd_spi);
	if (cmd != NULL) {
		command_add_param(cmd, "read", TYPE_BOOL);
		command_add_param(cmd, "write", TYPE_BOOL);
		command_add_param(cmd, "addr", TYPE_UINT32HEX);
		command_add_param(cmd, "value", TYPE_UINT32HEX);
	}
	
	cmd = register_command("filetest", cmd_filetest);
	
	extern uint8_t cli_rx_buffer[128];
	extern uint8_t usart_rx_buffer[128];
	extern uint8_t rx_flag;
	extern uint32_t rx_len;
	extern uint32_t last_cnt, cur_cnt;
	
	//while(1) osDelay(1);
	shell_exec();
}
