#ifndef __OLED_SSD1306_1_3_H
#define __OLED_SSD1306_1_3_H

#include <stdint.h>

#define OLED_DEVICE_ADDRESS 0x78

uint8_t OLED_Init(void);

/**
 * @brief OLED Send Control Command
 * 
 * @param cmd 
 */
uint8_t OLED_Write_Command(uint8_t cmd);

/**
 * @brief OLED Send Data
 * 
 * @param data 
 */
void OLED_Write_Data(uint8_t data);

/**
 * @brief OLED Set Cursor Position
 * 
 * @param x 0-127 From Left to Right
 * @param y 0-7 From Top to Bottom
 */
void OLED_Set_Cursor(uint8_t x, uint8_t y);

/**
 * @brief OLED Clear All Display
 * 
 */
void OLED_Clear(void);

/**
 * @brief OLED Show Char
 * 
 * @param row 1-4
 * @param column 1-16
 * @param ch 
 */
void OLED_Show_Char(uint8_t row, uint8_t column, char ch);

/**
 * @brief OLED Show String
 * 
 * @param row 1-4
 * @param column 1-16
 * @param str 
 */
void OLED_Show_String(uint8_t row, uint8_t column, const char* str);

/**
 * @brief OLED Show Number
 * 
 * @param row 1-4
 * @param column 1-16
 * @param num 
 */
void OLED_Show_Number(uint8_t row, uint8_t column, int32_t num);



#endif //__OLED_SSD1306_1_3_H
