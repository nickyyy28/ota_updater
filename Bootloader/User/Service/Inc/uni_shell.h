/**
 * @file usart_cli.h
 * @author nickyyy (huiyang.zhang@foxmail.com)
 * @brief this is a simple command line interface for stm32, sppported by usart or usbcdc
 *        used about 10k ram, 5k flash
 * @version 0.1
 * @date 2023-06-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __UNI_SHELL_H
#define __UNI_SHELL_H

#include <stdint.h>
#include <string.h>

#define CLI_MAX_COMMAND_COUNT 			20		//最多支持多少条指令
#define COMMAND_MAX_PARAM_COUNT			10		//每条指令最多支持多少参数
#define COMMAND_MAX_PARAM_KEY_SIZE		20		//参数名长度限制
#define COMMAND_MAX_PARAM_VALUE_SIZE	20		//参数值长度限制

//#define USE_CACHE

#define LOG_LEVEL_DEBUG 	1
#define LOG_LEVEL_INFO 		2
#define LOG_LEVEL_WARNING 	3
#define LOG_LEVEL_ERROR 	4
#define LOG_LEVEL_FATAL 	5

#define LOG_LEVEL	LOG_LEVEL_DEBUG

#define RX_BUFFSIZE	128				//串口或USBCDC接收缓冲区大小, 但是USBCDC一般单次数据包最多64字节, 注意使用方式

#define CURRSOR_BLINK		1		//光标是否闪烁
#define CURRSOR_BLINK_FRQ	3		//闪烁频率
#define CONSOLE_COLOR_ENABLE 1 		//是否使用彩色日志信息
#define CURRSOR_BLINK_TICK	(int)(1000 / CURRSOR_BLINK_FRQ * 2)

/* 键盘控制字符 */
#define KEY_ENTER		0x0D
#define KEY_TAB			0x09
#define KEY_BACKSPACE	0x08
#define KEY_UP_ARROW	0x1B5B41
#define KEY_DOWN_ARROW	0x1B5B42
#define KEY_LEFT_ARROW	0x1B5B44
#define KEY_RIGHT_ARROW	0x1B5B43

#define DELAY_TICK	5			//发送后的延迟函数, 如果使用串口传输,该参数可以不管, 如果使用USBCDC, 该参数酌情配置,单位ms

#define CONSOLE_COLOR_BLACK  "\033[30m"
#define CONSOLE_COLOR_RED  "\033[31m"
#define CONSOLE_COLOR_GREEN  "\033[32m"
#define CONSOLE_COLOR_YELLOW  "\033[33m"
#define CONSOLE_COLOR_BLUE  "\033[34m"
#define CONSOLE_COLOR_PURPLE  "\033[35m"
#define CONSOLE_COLOR_DEEP  "\033[36m"
#define CONSOLE_COLOR_WHITE  "\033[37m"

#define SHELL_DELAY osDelay

#if CONSOLE_COLOR_ENABLE == 1

//设置控制台字体颜色
#define SET_CONSOLE_COLOR(__color__) SHELL_Transmit((uint8_t*)__color__, strlen(__color__))
//恢复控制台字体颜色
#define RESUME_CONSOLE_COLOR() SET_CONSOLE_COLOR(CONSOLE_COLOR_WHITE)

#else

#define SET_CONSOLE_COLOR(__color__) 
#define RESUME_CONSOLE_COLOR()

#endif

//#define USE_USART	//使用串口
#define USE_USBCDC

#define USBCDC_MAX_PACKET_SIZE 64

#if defined USE_USBCDC
//终端发送函数
#define SHELL_Transmit(src, len)	\
	CDC_Transmit_Packet(src, len)
#elif defined USE_USART || defined USE_UART

//终端使用的串口
#define SHELL_USART	huart1

#define USART_IRQ_Handler USART1_IRQHandler

//终端发送函数
#define SHELL_Transmit(src, len)	HAL_UART_Transmit(&SHELL_USART, src, len, DELAY_TICK)

#endif

typedef enum{
	LOG_DEBUG = 1,
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
	LOG_FATAL
}_LOG_LEVEL;

/**
 * @brief 	参数类型
 * 
 */
typedef enum{
	TYPE_NONE = 0,		//未知类型
	TYPE_BOOL,			//布尔类型
	TYPE_CHAR,			//字符类型
	TYPE_SHORT,			//短整型
	TYPE_INT,			//整型
	TYPE_LONG,			//长整型
	TYPE_INT64,			//64位整型
	TYPE_FLOAT,			//浮点型
	TYPE_DOUBLE,		//双精度浮点型
	TYPE_UINT8HEX,		//8位无符号整型, 十六进制
	TYPE_UINT16HEX,		//16位无符号整型, 十六进制
	TYPE_UINT32HEX,		//32位无符号整型, 十六进制
	TYPE_UINT64HEX,		//64位无符号整型, 十六进制
	TYPE_STRING			//字符串
}Param_Type;

typedef enum{
	COMMAND_NO_ERR,		//无错误
	COMMAND_NOT_FOUND,	//未找到指令
	PARAM_NOT_FOUND,	//未找到参数
}CLI_ERR_TYPE;

//控制按键值
typedef union{
	uint32_t key_value;
	uint8_t buff[4];
}CONTROL_KEY_T;

//参数状态
typedef enum{
	NOT_CONFIGURED	= 0,	//未配置
	CONFIGURED				//已配置
}Param_Status;

//参数结构体
typedef struct{
	char key[COMMAND_MAX_PARAM_KEY_SIZE];		//参数名
	char value[COMMAND_MAX_PARAM_VALUE_SIZE];	//参数值
	Param_Type type;							//参数类型
	Param_Status status;		 				//参数状态
}Param_t;

//命令缓冲区
typedef struct{
	uint8_t buffer[256];						//命令缓冲区
	uint32_t size;								//缓冲区大小
	uint32_t insert_index;						//插入索引
}CMD_Buffer_t;

struct Command_t;

//命令回调函数
typedef void (*COMMAND_CALLBACK)(struct Command_t* cmd);

//命令结构体
typedef struct Command_t{
	const char* command;						//命令
	COMMAND_CALLBACK callback;					//回调函数
	Param_t params[COMMAND_MAX_PARAM_COUNT];	//参数
	uint32_t param_size;						//参数数量
}Command_t;

//注册命令结构体
typedef struct{
	Command_t commands[CLI_MAX_COMMAND_COUNT];		//命令
	uint32_t size;									//命令数量
}Register_Commands_t;

/**
 * @brief 	注册命令
 * 
 * @param cmd 			命令
 * @param callback 		回调函数
 * @return Command_t* 	命令结构体指针
 */
Command_t* register_command(const char* cmd, COMMAND_CALLBACK callback);

/**
 * @brief 	添加参数
 * 
 * @param cmd 			命令结构体指针
 * @param param_name  	参数名
 * @param type  		参数类型
 * @return Param_t*    	参数结构体指针
 */
Param_t* command_add_param(Command_t *cmd, const char* param_name, Param_Type type);

/**
 * @brief 	获取参数
 * 
 * @param cmd 			命令结构体指针
 * @param param_name  	参数名
 * @return Param_t*  	参数结构体指针
 */
Param_t* command_get_param(Command_t *cmd, const char* param_name);

/**
 * @brief 	参数是否已配置
 * 
 * @param cmd  			命令结构体指针
 * @param param_name  	参数名
 * @return uint8_t  	1:已配置 0:未配置
 */
uint8_t param_is_configured(Command_t *cmd, const char* param_name);

/**
 * @brief 	获取参数值
 * 
 * @param cmd  			命令结构体指针
 * @param param_name  	参数名
 * @param value  		参数值
 */
void command_get_param_value(Command_t *cmd, const char* param_name, void *value);

/**
 * @brief 	解码命令
 * 
 * @param src  			命令字符串
 * @param size  		命令字符串长度
 * @return Command_t*  	命令结构体指针
 */
Command_t* decode_command(uint8_t *src, uint32_t size);

/**
 * @brief  shell初始化
 * 
 */
void shell_init(void);

/**
 * @brief 	shell进入循环
 * 
 */
void shell_exec(void);

/**
 * @brief 	shell发送日志信息
 * 
 * @param level 日志等级
 * @param fmt  调试信息格式
 * @param ...  可变参数
 */
void SHELL_LOG_LEVEL(_LOG_LEVEL level, const char* fmt, ...);

/**
 * @brief 	shell发送数据
 * 
 * @param fmt 	数据格式
 * @param ...  	可变参数
 */
void SHELL_PRINTF(const char* fmt, ...);

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define	LOG_DEBUG(fmt, ...) SHELL_LOG_LEVEL(LOG_DEBUG, fmt, ##__VA_ARGS__)
#else
#define	LOG_DEBUG(...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_INFO
#define	LOG_INFO(fmt, ...) SHELL_LOG_LEVEL(LOG_INFO, fmt, ##__VA_ARGS__)
#else
#define	LOG_INFO(__VA_ARGS__)
#endif

#if LOG_LEVEL <= LOG_LEVEL_WARNING
#define	LOG_WARNING(fmt, ...) SHELL_LOG_LEVEL(LOG_WARNING, fmt, ##__VA_ARGS__)
#else
#define	LOG_WARNING(__VA_ARGS__)
#endif

#if LOG_LEVEL <= LOG_LEVEL_ERROR
#define	LOG_ERROR(fmt, ...) SHELL_LOG_LEVEL(LOG_ERROR, fmt, ##__VA_ARGS__)
#else
#define	LOG_ERROR(__VA_ARGS__)
#endif

#if LOG_LEVEL <= LOG_LEVEL_FATAL
#define	LOG_FATAL(fmt, ...) SHELL_LOG_LEVEL(LOG_FATAL, fmt, ##__VA_ARGS__)
#else
#define	LOG_FATAL(__VA_ARGS__)
#endif

#if defined USE_USBCDC
void CDC_Transmit_Packet(uint8_t *src, uint32_t len);
#endif

#endif
