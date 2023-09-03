#include "uni_shell.h"
#include <stdio.h>
#include <stdarg.h>
#include "usart.h"

#include <cmsis_os.h>

#ifdef USE_CACHE
__attribute__((section(".RW_IRAM2"))) __attribute__((aligned(32)))  uint8_t usart_rx_buffer[RX_BUFFSIZE] = {0};
#else
__attribute__((aligned(32))) uint8_t usart_rx_buffer[RX_BUFFSIZE] = {0};
#endif

Register_Commands_t cli_commands = {0};
CMD_Buffer_t cmd_buffer = {0};
CMD_Buffer_t last_cmd_buffer = {0};
uint32_t free_ticks = 0;

uint8_t shell_rx_buffer[RX_BUFFSIZE] = {0};
uint8_t rx_flag = 0;
uint32_t rx_len = 0;

void enter_callback(Command_t *cmd){}
Command_t enter_cmd = {
	.param_size = 0, 
	.params = {0}, 
	.callback = enter_callback,
	.command = "enter"
};

static void cmd_buffer_add_byte(uint8_t ch);

static void cmd_buffer_clear(void);

static void cmd_buffer_remove_byte(void);

static inline void clear_params(Command_t *cmd)
{
	for (int i = 0 ; i < cmd->param_size ; i++) {
		memset(cmd->params[i].value, 0, COMMAND_MAX_PARAM_VALUE_SIZE);
		cmd->params[i].status = NOT_CONFIGURED;
	}
}

Command_t* register_command(const char* cmd, COMMAND_CALLBACK callback)
{
	if (NULL == cmd || NULL == callback || 
		cli_commands.size >= CLI_MAX_COMMAND_COUNT) {
		return NULL;
	}
	
	Command_t* command = &cli_commands.commands[cli_commands.size++];
	
	command->callback = callback;
	command->command = cmd;
	
	return command;
}

Param_t* command_add_param(Command_t *cmd, const char* param_name, Param_Type type)
{
	if (NULL == cmd || NULL == param_name) {
		return NULL;
	}
	Param_t *param = &cmd->params[cmd->param_size++];

	strcpy(param->key, param_name);
	param->type = type;
	
	return param;
}

void command_get_param_value(Command_t *cmd, const char* param_name, void *value)
{
	if (NULL == cmd || NULL == param_name || NULL == value) {
		return;
	}
	
	Param_t *param = NULL;
	for (int i = 0 ; i < cmd->param_size ; i++) {
		param = &cmd->params[i];
		if (strcmp(param_name, param->key) == 0) {
#define CASES(__type__, __TYPE__, __fmt__) 					\
			case TYPE_##__TYPE__: {							\
				__type__ type_var = {0};							\
				sscanf(param->value, __fmt__, &type_var);	\
				*(__type__*)value = type_var;				\
				break;										\
			}
			
			switch (param->type) {
				case TYPE_BOOL: {
					if (param->value[0] == 'T' || param->value[0] == 't') {
						*(uint8_t*)value = 1;
					} else {
						*(uint8_t*)value = 0;
					}
					break;
				}
				CASES(char, CHAR, "%c");
				CASES(short, SHORT, "%hd");
				CASES(int, INT, "%d");
				CASES(long, LONG, "%ld");
				CASES(int64_t, INT64, "%lld");
				CASES(float, FLOAT, "%f");
				CASES(double, DOUBLE, "%lf");
				CASES(uint8_t, UINT8HEX, "0x%hhx");
				CASES(uint16_t, UINT16HEX, "0x%hx");
				CASES(uint32_t, UINT32HEX, "0x%x");
				CASES(uint64_t, UINT64HEX, "0x%llx");
				case TYPE_STRING: {
					if (strlen(param->value) == 0) {
						value = NULL;
					} else {
						*(int*)value = (int)&param->value;
					}
					break;
				}
				case TYPE_NONE: 
				default:	 {
					value = NULL;
					break;
				}
				
				
#undef CASES
			}
			return;
		}
		
	}
}

Command_t* decode_command(uint8_t *src, uint32_t size)
{
	if (src == NULL || size == 0) {
		return NULL;
	} 
	if (size == 1 && src[0] == 0x0D) {
		return &enter_cmd;
	}
	CLI_ERR_TYPE err_type = COMMAND_NO_ERR;
	char err_buff[64] = {0};
	char tmp_cmd[30] = {0};
	size_t index = 0;
	while (index < size && src[index] != '\0' && src[index] != ' ') {
		tmp_cmd[index] = src[index];
		index++;
	}
	
	Command_t *command = NULL;
	for (int i = 0 ; i < cli_commands.size ; i++) {
		if (strcmp(tmp_cmd, cli_commands.commands[i].command) == 0) {
			command = &cli_commands.commands[i];
			break;
		}
	}
	
	if (command == NULL) {
		err_type = COMMAND_NOT_FOUND;
		goto err;
	}
	
	
#define FIND_KEY 	1
#define GET_KEY		2
#define FIND_VALUE	3
#define GET_VALUE	4
#define FIND_KEY_OR_VALUE	5
	
	uint8_t status = FIND_KEY_OR_VALUE; 
	char key[COMMAND_MAX_PARAM_KEY_SIZE] = {0};
	char value[COMMAND_MAX_PARAM_VALUE_SIZE] = {0};
	int tmp_index = 0;
	uint8_t isStartGet = 0;
	Param_t *param = NULL;
	while (index < size && src[index] != '\n' && src[index] != '\0') {
			
		if (status == FIND_KEY_OR_VALUE) {
			if (src[index] == ' ') {
				index++;
				continue;
			}
			
			if (src[index] == '-') {
				index++;
				status = GET_KEY;
				continue;
			} else {
				status = GET_VALUE;
				continue;
			}
		}
		
		if (status == GET_KEY) {
			if (src[index] != ' ') {
				key[tmp_index] = src[index];
				tmp_index ++;
				index ++;
				if (index >= size || src[index] == '\0' || src[index] == '\n') {
					status = FIND_KEY_OR_VALUE;
					tmp_index = 0;
					param = command_get_param(command, key);
					if (param == NULL) {
						err_type = PARAM_NOT_FOUND;
						goto err;
					}
					param->status = CONFIGURED;
					memset(key, 0, strlen(key));
				}
				continue;
			} else {
				status = FIND_KEY_OR_VALUE;
				index++;
				tmp_index = 0;
				param = command_get_param(command, key);
				if (param == NULL) {
					err_type = PARAM_NOT_FOUND;
					goto err;
				}
				param->status = CONFIGURED;
				memset(key, 0, strlen(key));
				continue;
			}
		}
		
		if (status == GET_VALUE) {
			if (src[index] != ' ') {
				value[tmp_index] = src[index];
				tmp_index ++;
				index ++;
				
				if (index >= size || src[index] == '\0' || src[index] == '\n') {
					tmp_index = 0;
					status = FIND_KEY_OR_VALUE;
					if (param == NULL) {
						err_type = PARAM_NOT_FOUND;
						goto err;
					}
					strcpy(param->value, value);
					memset(key, 0, sizeof(key));
					memset(value, 0, sizeof(value));
					param = NULL;
				}
				continue;
			} else {
				tmp_index = 0;
				index ++;
				status = FIND_KEY_OR_VALUE;
				if (param == NULL) {
					err_type = PARAM_NOT_FOUND;
					goto err;
				}
				
				strcpy(param->value, value);
				memset(key, 0, sizeof(key));
				memset(value, 0, sizeof(value));
				param = NULL;
				continue;
			}
		}
		
	}
	
	return command;
err:
	if (command != NULL) {
		clear_params(command);
	}
	switch (err_type) {
		case COMMAND_NOT_FOUND: {
			sprintf(err_buff, "\r\nCommand: %s not found...", tmp_cmd);
			SHELL_Transmit(err_buff, strlen(err_buff));
			break;
		}
		case PARAM_NOT_FOUND: {
			sprintf(err_buff, "\r\nParam: %s not found...", key);
			SHELL_Transmit(err_buff, strlen(err_buff));
			break;
		}
		case COMMAND_NO_ERR:
		default:
			break;
	}
	
	SHELL_DELAY(DELAY_TICK);
	
	return NULL;
}

Param_t* command_get_param(Command_t *cmd, const char* param_name)
{
	Param_t *param = NULL;
	for (int i = 0 ; i < cmd->param_size ; i++) {
		if (strcmp(cmd->params[i].key, param_name) == 0) {
			param = &cmd->params[i];
			break;
		}
	}
	
	return param;
}

uint8_t param_is_configured(Command_t *cmd, const char* param_name)
{
	Param_t *param = NULL;
	for (int i = 0 ; i < cmd->param_size ; i++) {
		if (strcmp(cmd->params[i].key, param_name) == 0) {
			param = &cmd->params[i];
			break;
		}
	}
	
	if (param == NULL) {
		return NOT_CONFIGURED;
	}
	
	return param->status;
}

void cmd_buffer_add_byte(uint8_t ch)
{
	if (cmd_buffer.insert_index == cmd_buffer.size) {
		cmd_buffer.buffer[cmd_buffer.insert_index] = ch;
		cmd_buffer.insert_index++;
		cmd_buffer.size++;
	} else {
		for (int i = cmd_buffer.size - cmd_buffer.insert_index ; i > 0; i--) {
			cmd_buffer.buffer[cmd_buffer.insert_index + i] = cmd_buffer.buffer[cmd_buffer.insert_index + i - 1];
		}
		cmd_buffer.buffer[cmd_buffer.insert_index] = ch;
		cmd_buffer.insert_index++;
		cmd_buffer.size++;
	}
	
}

void shell_exec(void)
{
	SHELL_DELAY(1000);
	SHELL_Transmit("\r\n>", 3);
	while(1) {
		if (rx_flag) {
			//HAL_NVIC_DisableIRQ(USART1_IRQn);
			if (rx_len == 3) {
				CONTROL_KEY_T control_key = {0};
				control_key.buff[0] = shell_rx_buffer[2];
				control_key.buff[1] = shell_rx_buffer[1];
				control_key.buff[2] = shell_rx_buffer[0];
				switch (control_key.key_value) {
					case KEY_UP_ARROW:
						for (int i = 0 ; i < cmd_buffer.insert_index ; i++) {
							SHELL_Transmit("\033[D", 5);
						}
						SHELL_Transmit("\033[K", 5);
						//memcpy(&cmd_buffer, &last_cmd_buffer, sizeof(cmd_buffer));
						//SHELL_Transmit(cmd_buffer.buffer, cmd_buffer.size);
						//SHELL_DELAY(1);
					case KEY_DOWN_ARROW:
					case KEY_LEFT_ARROW:
						if (cmd_buffer.insert_index > 1) {
							cmd_buffer.insert_index--;
							SHELL_Transmit("\033[D", 5);
						}
						break;
					case KEY_RIGHT_ARROW:
						if (cmd_buffer.insert_index < cmd_buffer.size) {
							cmd_buffer.insert_index++;
							SHELL_Transmit("\033[C", 5);
						}
						break;
					default:
						break;
				}
				
				if (control_key.key_value >= KEY_UP_ARROW && control_key.key_value <= KEY_LEFT_ARROW) {
					rx_flag = 0;
					continue;
				}
			}
			for (int i = 0 ; i < rx_len ; i++) {
				uint8_t ch = shell_rx_buffer[i];
				switch (ch) {
					case KEY_ENTER:
						if (cmd_buffer.size != 0) {
							Command_t *cmd = NULL;
							cmd = decode_command(cmd_buffer.buffer, cmd_buffer.size);
							if (cmd != NULL) {
								cmd->callback(cmd);
								clear_params(cmd);
								SHELL_DELAY(DELAY_TICK);
							}
							memcpy(&last_cmd_buffer, &cmd_buffer, sizeof(cmd_buffer));
							cmd_buffer_clear();
							SHELL_Transmit("\r\n>", 3);
						} else {
							SHELL_Transmit("\r\n>", 3);
						}
						break;
					case KEY_TAB: {
						uint8_t isTypeCmd = 1;
						for (int i = 0 ; i < cmd_buffer.size ; i++) {
							if (cmd_buffer.buffer[i] == ' ' || cmd_buffer.buffer[i] == '\0' || cmd_buffer.buffer[i] == '\n') {
								isTypeCmd = 0;
								break;
							}
						}
						if (isTypeCmd) {
							Command_t* cmds[CLI_MAX_COMMAND_COUNT];
							int cmd_count = 0;
							for (int i = 0 ; i < cli_commands.size ; i++) {
								Command_t *cmd = &cli_commands.commands[i];
								
								int cur_len = cmd_buffer.size;
								int cmd_len = strlen(cmd->command);
								
								if (cur_len >= cmd_len) continue;
								
								uint8_t cmp_cmd[20] = {0};
								memcpy(cmp_cmd, cmd->command, cur_len);
								if (strcmp((const char*)cmd_buffer.buffer, (const char*)cmp_cmd) == 0) {
									cmds[cmd_count++] = cmd;
								}
							}
							if (cmd_count == 1) {
								cmd_buffer_clear();
								strcpy((char*)cmd_buffer.buffer, cmds[0]->command);
								cmd_buffer.size = cmd_buffer.insert_index = strlen(cmds[0]->command);
								SHELL_Transmit("\r\n>", 3);
								SHELL_Transmit(cmd_buffer.buffer, cmd_buffer.size);
							} else if (cmd_count > 1) {
								SHELL_Transmit("\r\n", 2);
								for (int i = 0 ; i < cmd_count ; i++) {
									char cmd_buff[20] = {0};
									sprintf(cmd_buff, "%s  ", cmds[i]->command);
									SHELL_Transmit(cmd_buff, strlen(cmd_buff));
								}
								SHELL_Transmit("\r\n>", 3);
								SHELL_Transmit(cmd_buffer.buffer, cmd_buffer.size);
							}
						}
						break;
					}
					case KEY_BACKSPACE:
						if (cmd_buffer.insert_index > 0) {
							cmd_buffer_remove_byte();
							SHELL_Transmit("\033[D", 5);
							SHELL_Transmit("\033[P", 5);
						}
						break;
					default:
						cmd_buffer_add_byte(ch);
						SHELL_Transmit(&cmd_buffer.buffer[cmd_buffer.insert_index - 1], cmd_buffer.size - cmd_buffer.insert_index + 1);
						int left_move = cmd_buffer.size - cmd_buffer.insert_index;
						for (int i = 0 ; i < left_move ; i++) {
							SHELL_Transmit("\033[D", 5);
						}
						break;
				}
			}
			rx_flag = 0;
			free_ticks = 0;
			memset(shell_rx_buffer, 0, rx_len);
			//HAL_NVIC_EnableIRQ(USART1_IRQn);
		} else {
#if CURRSOR_BLINK
			free_ticks++;
			free_ticks %= CURRSOR_BLINK_TICK;
			if (free_ticks == 0) {
				SHELL_Transmit("\033[?25l", 8);
			} else if (free_ticks == CURRSOR_BLINK_TICK / 2) {
				SHELL_Transmit("\033[?25h", 8);
			} else {
				SHELL_DELAY(1);
			}
#else 
			SHELL_DELAY(DELAY_TICK);
#endif
			
		}
	}
}

void cmd_buffer_clear(void)
{
	memset(cmd_buffer.buffer, 0, cmd_buffer.size);
	cmd_buffer.size = 0;
	cmd_buffer.insert_index = 0;
}

void cmd_buffer_remove_byte(void)
{
	if (cmd_buffer.insert_index == cmd_buffer.size) {
		cmd_buffer.size--;
		cmd_buffer.insert_index--;
		cmd_buffer.buffer[cmd_buffer.size] = 0;
	} else {
		cmd_buffer.size--;
		cmd_buffer.insert_index--;
		for (int i = 0 ; i < cmd_buffer.size - cmd_buffer.insert_index; i++) {
			cmd_buffer.buffer[cmd_buffer.insert_index + i] = cmd_buffer.buffer[cmd_buffer.insert_index + i + 1];
		}
	}
}

void cmd_clear(Command_t *cmd)
{
	SHELL_Transmit("\033[0;0H", 8);
	SHELL_DELAY(DELAY_TICK);
	SHELL_Transmit("\033[2J", 6);
	SHELL_DELAY(DELAY_TICK);
}

void cmd_logo(Command_t *cmd)
{
	SHELL_Transmit("\r\n   __  __      _ _____ __         ____", 41);
	SHELL_Transmit("\r\n  / / / /___  (_) ___// /_  ___  / / /", 41);
	SHELL_Transmit("\r\n / / / / __ \\/ /\\__ \\/ __ \\/ _ \\/ / /", 40);
	SHELL_Transmit("\r\n/ /_/ / / / / /___/ / / / /  __/ / /", 39);
	SHELL_Transmit("\r\n\\____/_/ /_/_//____/_/ /_/\\___/_/_/\r\n", 40);
}

void cmd_reboot(Command_t *cmd)
{
	//register uint32_t __regFaultMask       __ASM("faultmask");
	//__regFaultMask = (__regFaultMask & 1);
	HAL_NVIC_SystemReset();
}

void shell_init(void)
{
	/*while (hUsbDeviceFS.dev_state != USBD_STATE_CONFIGURED) {
		SHELL_DELAY(1);
	}*/
	SHELL_DELAY(2000);
	cmd_logo(NULL);
	SHELL_DELAY(1000);
	
#if defined USE_USART || defined USE_UART
	
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart1, usart_rx_buffer, RX_BUFFSIZE);
	
#endif
	
	register_command("logo", cmd_logo);
	register_command("clear", cmd_clear);
	register_command("reboot", cmd_reboot);
}

void SHELL_LOG(const char* fmt, ...)
{
	static char log_buffer[150] = {0}, raw_buffer[128] = {0};
	// memset(raw_buffer, 0, 128);
	// memset(log_buffer, 0, 150);
	
	va_list args;
    va_start(args, fmt);
    
    vsprintf(raw_buffer, fmt, args);
	uint32_t cur_tick = HAL_GetTick();
	uint32_t hours = 0, minutes = 0, seconds = 0, milliseconds = 0;
	//milliseconds = cur_tick % 1000;
	hours = cur_tick / (1000 * 60 * 60);
	cur_tick %= (1000 * 60 * 60);
	minutes = cur_tick / (1000 * 60);
	cur_tick %= (1000 * 60);
	seconds = cur_tick / 1000;
	sprintf(log_buffer, "\r\n[%02u:%02u:%02u]: %s", hours, minutes, seconds, raw_buffer);
    
    va_end(args);
	SET_CONSOLE_COLOR(CONSOLE_COLOR_GREEN);
	SHELL_Transmit(log_buffer, strlen(log_buffer));
	RESUME_CONSOLE_COLOR();
}

void SHELL_DEBUG(const char* fmt, ...)
{
	static char log_buffer[300] = {0}, raw_buffer[200] = {0};
	//memset(raw_buffer, 0, 300);
	//memset(log_buffer, 0, 200);
	
	va_list args;
    va_start(args, fmt);
    
    vsprintf(raw_buffer, fmt, args);
	uint32_t cur_tick = HAL_GetTick();
	uint32_t hours = 0, minutes = 0, seconds = 0, milliseconds = 0;
	//milliseconds = cur_tick % 1000;
	hours = cur_tick / (1000 * 60 * 60);
	cur_tick %= (1000 * 60 * 60);
	minutes = cur_tick / (1000 * 60);
	cur_tick %= (1000 * 60);
	seconds = cur_tick / 1000;
	sprintf(log_buffer, "\r\n[%02u:%02u:%02u]: %s", hours, minutes, seconds, raw_buffer);
    
    va_end(args);
	SET_CONSOLE_COLOR(CONSOLE_COLOR_BLUE);
	SHELL_Transmit(log_buffer, strlen(log_buffer));
	RESUME_CONSOLE_COLOR();
}

void SHELL_PRINTF(const char* fmt, ...)
{
	char print_buffer[128] = {0};
	
	va_list args;
    va_start(args, fmt);

	vsprintf(print_buffer, fmt, args);

	va_end(args);

	SHELL_Transmit(print_buffer, strlen(print_buffer));
}

#if defined USE_USART || USB_UART

void USART_IRQ_Handler(void)
{
	if (RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE)) {
		__HAL_UART_CLEAR_IDLEFLAG(&huart1);
#ifdef USE_CACHE
		SCB_InvalidateDCache_by_Addr(usart_rx_buffer, 128);
#endif
		HAL_UART_DMAStop(&huart1);
		rx_len = 128 - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
		memcpy(shell_rx_buffer, usart_rx_buffer, rx_len);	
		rx_flag = 1;
		HAL_UART_Receive_DMA(&huart1, usart_rx_buffer, 128);
	}
}

#else

void CDC_RECEIVE(uint8_t* src, uint32_t len)
{
		rx_len = len;
		memcpy(shell_rx_buffer, src, len);
		rx_flag = 1;
}

#if defined USE_USBCDC
void CDC_Transmit_Packet(uint8_t *src, uint32_t len)
{
	static uint8_t buffer[USBCDC_MAX_PACKET_SIZE] = {0};
	for (int i = 0 ; i < len ; i += USBCDC_MAX_PACKET_SIZE ) {
		if (len - i >= USBCDC_MAX_PACKET_SIZE) {
			CDC_Transmit_FS(src + i, USBCDC_MAX_PACKET_SIZE);
		}  else {
			CDC_Transmit_FS(src + i, len - i);
		}
		SHELL_DELAY(DELAY_TICK);
	}
}
#endif

#endif