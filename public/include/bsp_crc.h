#ifndef __BSP_CRC_H
#define __BSP_CRC_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 计算CRC8
 * 
 * @param pchMessage    数据源指针
 * @param dwLength      数据长度
 * @param ucCRC8        CRC输入
 * @return uint8_t      计算得到的CRC
 */
uint8_t CRC8_calculate(unsigned char *pchMessage, unsigned int dwLength, unsigned char ucCRC8);

/**
 * @brief 计算CRC16
 * 
 * @param pchMessage    数据源指针
 * @param dwLength      数据长度
 * @param wCRC          CRC输入
 * @return uint16_t     计算得到的CRC
 */
uint16_t CRC16_calculate(uint8_t *pchMessage, uint32_t dwLength, uint16_t wCRC);

#ifdef __cplusplus
}
#endif

#endif //__BSP_CRC_H
