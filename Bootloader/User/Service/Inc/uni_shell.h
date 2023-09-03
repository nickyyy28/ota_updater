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

#define CLI_MAX_COMMAND_COUNT 			20		//���֧�ֶ�����ָ��
#define COMMAND_MAX_PARAM_COUNT			10		//ÿ��ָ�����֧�ֶ��ٲ���
#define COMMAND_MAX_PARAM_KEY_SIZE		20		//��������������
#define COMMAND_MAX_PARAM_VALUE_SIZE	20		//����ֵ��������

//#define USE_CACHE

#define RX_BUFFSIZE	128				//���ڻ�USBCDC���ջ�������С, ����USBCDCһ�㵥�����ݰ����64�ֽ�, ע��ʹ�÷�ʽ

#define CURRSOR_BLINK		1		//����Ƿ���˸
#define CURRSOR_BLINK_FRQ	3		//��˸Ƶ��
#define CONSOLE_COLOR_ENABLE 0 //
#define CURRSOR_BLINK_TICK	(int)(1000 / CURRSOR_BLINK_FRQ * 2)

/* ���̿����ַ� */
#define KEY_ENTER		0x0D
#define KEY_TAB			0x09
#define KEY_BACKSPACE	0x08
#define KEY_UP_ARROW	0x1B5B41
#define KEY_DOWN_ARROW	0x1B5B42
#define KEY_LEFT_ARROW	0x1B5B44
#define KEY_RIGHT_ARROW	0x1B5B43

#define DELAY_TICK	5			//���ͺ���ӳٺ���, ���ʹ�ô��ڴ���,�ò������Բ���, ���ʹ��USBCDC, �ò�����������,��λms

#define COLORFUL_LOG_INFO	1	//�Ƿ�ʹ�ò�ɫ��־��Ϣ
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

//���ÿ���̨������ɫ
#define SET_CONSOLE_COLOR(__color__) SHELL_Transmit(__color__, strlen(__color__))
//�ָ�����̨������ɫ
#define RESUME_CONSOLE_COLOR() SET_CONSOLE_COLOR(CONSOLE_COLOR_WHITE)

#else

#define SET_CONSOLE_COLOR(__color__) 
#define RESUME_CONSOLE_COLOR()

#endif

//#define USE_USART	//ʹ�ô���
#define USE_USBCDC

#define USBCDC_MAX_PACKET_SIZE 64

#if defined USE_USBCDC
//�ն˷��ͺ���
#define SHELL_Transmit(src, len)	\
	CDC_Transmit_Packet(src, len)
#elif defined USE_USART || defined USE_UART

//�ն�ʹ�õĴ���
#define SHELL_USART	huart1

#define USART_IRQ_Handler USART1_IRQHandler

//�ն˷��ͺ���
#define SHELL_Transmit(src, len)	HAL_UART_Transmit(&SHELL_USART, src, len, DELAY_TICK)

#endif

/**
 * @brief 	��������
 * 
 */
typedef enum{
	TYPE_NONE = 0,		//δ֪����
	TYPE_BOOL,			//��������
	TYPE_CHAR,			//�ַ�����
	TYPE_SHORT,			//������
	TYPE_INT,			//����
	TYPE_LONG,			//������
	TYPE_INT64,			//64λ����
	TYPE_FLOAT,			//������
	TYPE_DOUBLE,		//˫���ȸ�����
	TYPE_UINT8HEX,		//8λ�޷�������, ʮ������
	TYPE_UINT16HEX,		//16λ�޷�������, ʮ������
	TYPE_UINT32HEX,		//32λ�޷�������, ʮ������
	TYPE_UINT64HEX,		//64λ�޷�������, ʮ������
	TYPE_STRING			//�ַ���
}Param_Type;

typedef enum{
	COMMAND_NO_ERR,		//�޴���
	COMMAND_NOT_FOUND,	//δ�ҵ�ָ��
	PARAM_NOT_FOUND,	//δ�ҵ�����
}CLI_ERR_TYPE;

//���ư���ֵ
typedef union{
	uint32_t key_value;
	uint8_t buff[4];
}CONTROL_KEY_T;

//����״̬
typedef enum{
	NOT_CONFIGURED	= 0,	//δ����
	CONFIGURED				//������
}Param_Status;

//�����ṹ��
typedef struct{
	char key[COMMAND_MAX_PARAM_KEY_SIZE];		//������
	char value[COMMAND_MAX_PARAM_VALUE_SIZE];	//����ֵ
	Param_Type type;							//��������
	Param_Status status;						//����״̬
}Param_t;

//�������
typedef struct{
	uint8_t buffer[256];						//�������
	uint32_t size;								//��������С
	uint32_t insert_index;						//��������
}CMD_Buffer_t;

struct Command_t;

//����ص�����
typedef void (*COMMAND_CALLBACK)(struct Command_t* cmd);

//����ṹ��
typedef struct Command_t{
	const char* command;						//����
	COMMAND_CALLBACK callback;					//�ص�����
	Param_t params[COMMAND_MAX_PARAM_COUNT];	//����
	uint32_t param_size;						//��������
}Command_t;

//ע������ṹ��
typedef struct{
	Command_t commands[CLI_MAX_COMMAND_COUNT];		//����
	uint32_t size;									//��������
}Register_Commands_t;

/**
 * @brief 	ע������
 * 
 * @param cmd 			����
 * @param callback 		�ص�����
 * @return Command_t* 	����ṹ��ָ��
 */
Command_t* register_command(const char* cmd, COMMAND_CALLBACK callback);

/**
 * @brief 	��Ӳ���
 * 
 * @param cmd 			����ṹ��ָ��
 * @param param_name  	������
 * @param type  		��������
 * @return Param_t*    	�����ṹ��ָ��
 */
Param_t* command_add_param(Command_t *cmd, const char* param_name, Param_Type type);

/**
 * @brief 	��ȡ����
 * 
 * @param cmd 			����ṹ��ָ��
 * @param param_name  	������
 * @return Param_t*  	�����ṹ��ָ��
 */
Param_t* command_get_param(Command_t *cmd, const char* param_name);

/**
 * @brief 	�����Ƿ�������
 * 
 * @param cmd  			����ṹ��ָ��
 * @param param_name  	������
 * @return uint8_t  	1:������ 0:δ����
 */
uint8_t param_is_configured(Command_t *cmd, const char* param_name);

/**
 * @brief 	��ȡ����ֵ
 * 
 * @param cmd  			����ṹ��ָ��
 * @param param_name  	������
 * @param value  		����ֵ
 */
void command_get_param_value(Command_t *cmd, const char* param_name, void *value);

/**
 * @brief 	��������
 * 
 * @param src  			�����ַ���
 * @param size  		�����ַ�������
 * @return Command_t*  	����ṹ��ָ��
 */
Command_t* decode_command(uint8_t *src, uint32_t size);

/**
 * @brief  shell��ʼ��
 * 
 */
void shell_init(void);

/**
 * @brief 	shell����ѭ��
 * 
 */
void shell_exec(void);

/**
 * @brief 	shell������־
 * 
 * @param fmt  ��־��ʽ
 * @param ...  �ɱ����
 */
void SHELL_LOG(const char* fmt, ...);

/**
 * @brief 	shell���͵�����Ϣ
 * 
 * @param fmt  ������Ϣ��ʽ
 * @param ...  �ɱ����
 */
void SHELL_DEBUG(const char* fmt, ...);

/**
 * @brief 	shell��������
 * 
 * @param fmt 	���ݸ�ʽ
 * @param ...  	�ɱ����
 */
void SHELL_PRINTF(const char* fmt, ...);

#if defined USE_USBCDC
void CDC_Transmit_Packet(uint8_t *src, uint32_t len);
#endif

#endif
