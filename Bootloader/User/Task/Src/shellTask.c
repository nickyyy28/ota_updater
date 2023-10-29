#include "shellTask.h"

#include "main.h"
#include "usbd_cdc_if.h"
#include "uni_shell.h"
#include "usb_device.h"
#include <string.h>
#include "usart.h"
#include "w25qxx_driver.h"
//#include "w25qxx_4M.h"
#include "soft_i2c.h"
#include "eeprom_at24c256.h"
#include "oled_ssd1306_1_3.h"
#include "Device2Client.h"
#include "queue.h"

extern QueueHandle_t ota_cmd_queue;
W25Qxx flash1 = {0};

extern void FileTest(void); 

char msg_info[20] = {0};

void show_msg(Command_t *cmd)
{
	int var1 = 0;
	float var2 = 0.0f;
	char ch = ' ';
	memset(msg_info, 0, 20);
	command_get_param_value(cmd, "fvar", &var2);
	command_get_param_value(cmd, "ivar", &var1);
	command_get_param_value(cmd, "strvar", &msg_info);
	command_get_param_value(cmd, "cvar", &ch);

	LOG_INFO("show ivar=%d, fvar=%f, ch = %c", var1, var2, ch);
	LOG_INFO("str value = %s", msg_info);
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

void cmd_ota_test(Command_t *cmd)
{
	BaseType_t ret;
	uint16_t ota_cmd;
	if (param_is_configured(cmd, "cmd")) {
		command_get_param_value(cmd, "cmd", &ota_cmd);
		if (!MSG_ID_CHECK(ota_cmd)) {
			LOG_ERROR("cmd %hx not exist", ota_cmd);
			return;
		}
		LOG_DEBUG("sending ota cmd: 0x%04hx to queue", ota_cmd);
		ret = xQueueSendToBack(ota_cmd_queue, &ota_cmd, 100);
		if (pdPASS != ret) {
			LOG_ERROR("ota queue send failure");
			return;
		} else {
			LOG_INFO("ota queue send success");
		}
	} else {
		LOG_WARNING("param <cmd> is not configured");
	}
}

#include "w25qxx_driver.h"

uint8_t eeprom_str[64] = {0};

uint8_t w25qxx_read_buffer[256] = {0};
uint8_t w25qxx_write_buffer[256] = {0};

void cmd_test_write(Command_t *cmd)
{
	uint32_t page_addr = 0;
	memset(w25qxx_write_buffer, 0, 256);
	if (param_is_configured(cmd, "page") && param_is_configured(cmd, "str")) {
		command_get_param_value(cmd, "page", &page_addr);
		command_get_param_value(cmd, "str", &w25qxx_write_buffer);
		LOG_INFO("writing \"%s\" on page %d without erase", w25qxx_write_buffer, page_addr);
		w25qxx_write_page(&flash1, page_addr, w25qxx_write_buffer, strlen((const char*)w25qxx_write_buffer));
	} else {
		LOG_WARNING("command test_write lack of param");
	}
}

void cmd_test_read(Command_t *cmd)
{
	uint32_t page_addr = 0;
	uint32_t len = 0;
	memset(w25qxx_read_buffer, 0, 256);
	if (param_is_configured(cmd, "page") && param_is_configured(cmd, "len")) {
		command_get_param_value(cmd, "page", &page_addr);
		command_get_param_value(cmd, "len", &len);
		LOG_INFO("reading data on page %d", page_addr);
		w25qxx_read_page(&flash1, page_addr, w25qxx_read_buffer, len); 
		/*for (int i = 0 ; i < 16 ; i++) {
			LOG_DEBUG("0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X", 
				w25qxx_read_buffer[i * 8 + 0], w25qxx_read_buffer[i * 8 + 1], w25qxx_read_buffer[i * 8 + 2], w25qxx_read_buffer[i * 8 + 3],
				w25qxx_read_buffer[i * 8 + 4], w25qxx_read_buffer[i * 8 + 5], w25qxx_read_buffer[i * 8 + 6], w25qxx_read_buffer[i * 8 + 7],
				w25qxx_read_buffer[i * 8 + 8], w25qxx_read_buffer[i * 8 + 9], w25qxx_read_buffer[i * 8 + 10], w25qxx_read_buffer[i * 8 + 11],
				w25qxx_read_buffer[i * 8 + 12], w25qxx_read_buffer[i * 8 + 13], w25qxx_read_buffer[i * 8 + 14], w25qxx_read_buffer[i * 8 + 15]);
		}*/
		LOG_INFO("read str: %s", w25qxx_read_buffer);
	} else {
		LOG_WARNING("command test_read lack of param");
	}
}

void cmd_test_erase(Command_t *cmd)
{
	uint32_t sector_addr = 0;
	if (param_is_configured(cmd, "sector")) {
		command_get_param_value(cmd, "sector", &sector_addr);
		LOG_INFO("erasing sector %d", sector_addr);
		w25qxx_erase_sector(&flash1, sector_addr);
	}
}

void cmd_filetest(Command_t *cmd)
{
	//FileTest();
	/*w25qxx_read_id(&flash1);
	LOG_DEBUG("-------------------------");
	w25qxx_read_JEDEC_id(&flash1);
	LOG_DEBUG("-------------------------");
	w25qxx_read_status_register(&flash1, 1);
	w25qxx_read_status_register(&flash1, 2);
	w25qxx_read_status_register(&flash1, 3);


	LOG_INFO("reg1: %X, reg2: %X, reg3: %X", flash1.StatusRegister1.buffer, flash1.StatusRegister2.buffer, flash1.StatusRegister3.buffer);

	uint8_t ret = 0;
	LOG_DEBUG("erasing sector 0");*/
	//ret = w25qxx_erase_sector(&flash1, 0);
	
	//ret = w25qxx_write_page(&flash1, 0, "hello world", 11);
	
	/*ret = w25qxx_read_page(&flash1, 0, w25qxx_read_buffer, 256);
	LOG_DEBUG("ret = %d", ret);
	for (int i = 0 ; i < 16 ; i++) {
		LOG_DEBUG("0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X", 
			w25qxx_read_buffer[i * 8 + 0], w25qxx_read_buffer[i * 8 + 1], w25qxx_read_buffer[i * 8 + 2], w25qxx_read_buffer[i * 8 + 3],
			w25qxx_read_buffer[i * 8 + 4], w25qxx_read_buffer[i * 8 + 5], w25qxx_read_buffer[i * 8 + 6], w25qxx_read_buffer[i * 8 + 7],
			w25qxx_read_buffer[i * 8 + 8], w25qxx_read_buffer[i * 8 + 9], w25qxx_read_buffer[i * 8 + 10], w25qxx_read_buffer[i * 8 + 11],
			w25qxx_read_buffer[i * 8 + 12], w25qxx_read_buffer[i * 8 + 13], w25qxx_read_buffer[i * 8 + 14], w25qxx_read_buffer[i * 8 + 15]);
	}*/

	/*W25qxx_Init();
	memset(w25qxx_buffer, 0, 256);
	W25qxx_ReadPage(w25qxx_buffer, 0, 0, 256);
	LOG_DEBUG("other read:");
	for (int i = 0 ; i < 16 ; i++) {
		LOG_DEBUG("0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X", 
			w25qxx_buffer[i * 8 + 0], w25qxx_buffer[i * 8 + 1], w25qxx_buffer[i * 8 + 2], w25qxx_buffer[i * 8 + 3],
			w25qxx_buffer[i * 8 + 4], w25qxx_buffer[i * 8 + 5], w25qxx_buffer[i * 8 + 6], w25qxx_buffer[i * 8 + 7],
			w25qxx_buffer[i * 8 + 8], w25qxx_buffer[i * 8 + 9], w25qxx_buffer[i * 8 + 10], w25qxx_buffer[i * 8 + 11],
			w25qxx_buffer[i * 8 + 12], w25qxx_buffer[i * 8 + 13], w25qxx_buffer[i * 8 + 14], w25qxx_buffer[i * 8 + 15]);
	}*/

	/*for (int page_index = 0 ; page_index < 32 ; page_index++) {
		ret = w25qxx_read_page(&flash1, 1, w25qxx_buffer, 256);
		LOG_DEBUG("ret = %d", ret);
		LOG_DEBUG("read page: %d data: ", page_index);
		for (int i = 0 ; i < 16 ; i++) {
			LOG_DEBUG("0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X", 
				w25qxx_buffer[i * 8 + 0], w25qxx_buffer[i * 8 + 1], w25qxx_buffer[i * 8 + 2], w25qxx_buffer[i * 8 + 3],
				w25qxx_buffer[i * 8 + 4], w25qxx_buffer[i * 8 + 5], w25qxx_buffer[i * 8 + 6], w25qxx_buffer[i * 8 + 7],
				w25qxx_buffer[i * 8 + 8], w25qxx_buffer[i * 8 + 9], w25qxx_buffer[i * 8 + 10], w25qxx_buffer[i * 8 + 11],
				w25qxx_buffer[i * 8 + 12], w25qxx_buffer[i * 8 + 13], w25qxx_buffer[i * 8 + 14], w25qxx_buffer[i * 8 + 15]);
		}
	}*/
	
	



	/*RCC_ClkInitTypeDef clkConfig;
	uint32_t frq;
	HAL_RCC_GetClockConfig(&clkConfig, FLASH_LATENCY_0);
	frq = HAL_RCC_GetSysClockFreq();
	LOG_DEBUG("CPU Frq %d Hz", frq);*/
	
	LOG_INFO("writing string to eeprom");
	uint32_t start = HAL_GetTick();
	eeprom_write_str(0xA0, 0, 0, "test", 4);
	start = HAL_GetTick() - start;
	LOG_INFO("write cost %d ms", start);
	osDelay(100);
	LOG_INFO("reading string from eeprom");
	eeprom_read_page(0xA0, 0, 0, eeprom_str, 4);
	LOG_INFO("read str %s", eeprom_str);
	
	/*OLED_Init();
	LOG_INFO("OLED Init...");
	osDelay(1000);
	LOG_INFO("OLED Clear");
	OLED_Clear();
	osDelay(1000);
	OLED_Show_Char(1, 1, 'A');
	LOG_INFO("OLED Show Char");*/
}

void shell_task(void* param)
{
	shell_init();
	IIC_Init();
	osDelay(100);
	OLED_Init();
	w25qxx_init(&flash1);
	//W25qxx_Init();
	//MX_USB_DEVICE_Init();
	Command_t *cmd = register_command("show", show_msg);
	if (cmd != NULL) {
		command_add_param(cmd, "fvar", TYPE_FLOAT);
		command_add_param(cmd, "ivar", TYPE_INT);
		command_add_param(cmd, "strvar", TYPE_STRING);
		command_add_param(cmd, "cvar", TYPE_CHAR);
	}
	
	cmd = register_command("setled", cmd_setled);
	if (cmd != NULL) {
		command_add_param(cmd, "led1", TYPE_BOOL);
	}
	
	cmd = register_command("test_write", cmd_test_write);
	if (cmd != NULL) {
		command_add_param(cmd, "page", TYPE_INT);
		command_add_param(cmd, "str", TYPE_STRING);
	}
	
	cmd = register_command("test_read", cmd_test_read);
	if (cmd != NULL) {
		command_add_param(cmd, "page", TYPE_INT);
		command_add_param(cmd, "len", TYPE_INT);
	}
	
	cmd = register_command("test_erase", cmd_test_erase);
	if (cmd != NULL) {
		command_add_param(cmd, "sector", TYPE_INT);
	}
	
	cmd = register_command("ota_test", cmd_ota_test);
	if (cmd != NULL) {
		command_add_param(cmd, "cmd", TYPE_UINT16HEX);
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
