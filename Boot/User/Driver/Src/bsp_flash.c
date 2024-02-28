#include "bsp_flash.h"

#include "stm32h7xx_hal.h"
#include <string.h>

static __attribute__((aligned(8))) uint8_t erase_buffer[512] = {0};

#if ENABLE_DEBUG_INFO

#include "usart.h"
#include <stdio.h>
char buffer[128] = {0};
#endif

#define FLAG_ENABLE 	1
#define FLAG_DISABLE	0
	
FlashUpdateResult set_program_flag(uint8_t enable)
{
	uint32_t addr = 0;
	uint32_t value = PROGRAM_FLAG_ENABLE;
	
	memcpy(erase_buffer, (const void*)(PROGRAM_SECTOR_ADDRESS), sizeof(erase_buffer));
	
	if (enable == FLAG_ENABLE) {
		*(volatile uint32_t *)&erase_buffer[PROGRAM_FLAG_ADDRESS - PROGRAM_SECTOR_ADDRESS] = PROGRAM_FLAG_ENABLE;
	} else if (enable == FLAG_DISABLE){
		*(volatile uint32_t *)&erase_buffer[PROGRAM_FLAG_ADDRESS - PROGRAM_SECTOR_ADDRESS] = PROGRAM_FLAG_DISABLE;
	} else {
		return UpdateFail;
	}
	
	HAL_FLASH_Unlock();
	
#if ENABLE_DEBUG_INFO
	sprintf(buffer, "FLASH unlocked");
	HAL_UART_Transmit(&huart1, buffer, strlen(buffer), 10);
	memset(buffer, 0, strlen(buffer));
#endif
	
	static FLASH_EraseInitTypeDef eraseInit;
	eraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
	eraseInit.NbSectors = 1;
	eraseInit.Banks = FLASH_BANK_1;
	eraseInit.Sector = 2;
	eraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	uint32_t sectorErr = 0;
	
	if (HAL_FLASHEx_Erase(&eraseInit, &sectorErr) != HAL_OK) {
		return UpdateFail;
	}
	
#if ENABLE_DEBUG_INFO
	sprintf(buffer, "Erase FLASH Finished");
	HAL_UART_Transmit(&huart1, buffer, strlen(buffer), 10);
	memset(buffer, 0, strlen(buffer));
#endif
	
	addr = PROGRAM_SECTOR_ADDRESS;
	while(addr < PROGRAM_SECTOR_ADDRESS + sizeof(erase_buffer)) {
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, addr, (uint32_t)((&erase_buffer[addr]))) == HAL_OK) {
			addr += 32;
		} else {
			return UpdateFail;
		}
	}
	
	HAL_FLASH_Lock();
	
#if ENABLE_DEBUG_INFO
	sprintf(buffer, "FLASH locked");
	HAL_UART_Transmit(&huart1, buffer, strlen(buffer), 10);
	memset(buffer, 0, strlen(buffer));
#endif
	
	uint8_t memCheckRet = 0;
	
	addr = PROGRAM_SECTOR_ADDRESS;
	while(addr < PROGRAM_SECTOR_ADDRESS + sizeof(erase_buffer) && memCheckRet == 0) {
		if (*(volatile uint32_t *)(addr) != *(uint32_t*)(&erase_buffer[addr])) {
			memCheckRet = 1;
		}
		addr += 4;
	}
	
	if (memCheckRet) {
#if ENABLE_DEBUG_INFO
	sprintf(buffer, "FLASH check failed");
	HAL_UART_Transmit(&huart1, buffer, strlen(buffer), 10);
	memset(buffer, 0, strlen(buffer));
#endif
		return UpdateFail;
	}
	
#if ENABLE_DEBUG_INFO
	sprintf(buffer, "FLASH check successed");
	HAL_UART_Transmit(&huart1, buffer, strlen(buffer), 10);
	memset(buffer, 0, strlen(buffer));
#endif
	
	return UpdateSuccess;
}

uint8_t program_flag_is_enable(void)
{
	uint32_t value = *(volatile uint32_t *)(PROGRAM_FLAG_ADDRESS);
	return !!(value == PROGRAM_FLAG_ENABLE);
}

uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if(((Address < ADDR_FLASH_SECTOR_1_BANK1) && (Address >= ADDR_FLASH_SECTOR_0_BANK1)) || \
     ((Address < ADDR_FLASH_SECTOR_1_BANK2) && (Address >= ADDR_FLASH_SECTOR_0_BANK2)))    
  {
    sector = FLASH_SECTOR_0;  
  }
  else if(((Address < ADDR_FLASH_SECTOR_2_BANK1) && (Address >= ADDR_FLASH_SECTOR_1_BANK1)) || \
          ((Address < ADDR_FLASH_SECTOR_2_BANK2) && (Address >= ADDR_FLASH_SECTOR_1_BANK2)))    
  {
    sector = FLASH_SECTOR_1;  
  }
  else if(((Address < ADDR_FLASH_SECTOR_3_BANK1) && (Address >= ADDR_FLASH_SECTOR_2_BANK1)) || \
          ((Address < ADDR_FLASH_SECTOR_3_BANK2) && (Address >= ADDR_FLASH_SECTOR_2_BANK2)))    
  {
    sector = FLASH_SECTOR_2;  
  }
  else if(((Address < ADDR_FLASH_SECTOR_4_BANK1) && (Address >= ADDR_FLASH_SECTOR_3_BANK1)) || \
          ((Address < ADDR_FLASH_SECTOR_4_BANK2) && (Address >= ADDR_FLASH_SECTOR_3_BANK2)))    
  {
    sector = FLASH_SECTOR_3;  
  }
  else if(((Address < ADDR_FLASH_SECTOR_5_BANK1) && (Address >= ADDR_FLASH_SECTOR_4_BANK1)) || \
          ((Address < ADDR_FLASH_SECTOR_5_BANK2) && (Address >= ADDR_FLASH_SECTOR_4_BANK2)))    
  {
    sector = FLASH_SECTOR_4;  
  }
  else if(((Address < ADDR_FLASH_SECTOR_6_BANK1) && (Address >= ADDR_FLASH_SECTOR_5_BANK1)) || \
          ((Address < ADDR_FLASH_SECTOR_6_BANK2) && (Address >= ADDR_FLASH_SECTOR_5_BANK2)))    
  {
    sector = FLASH_SECTOR_5;  
  }
  else if(((Address < ADDR_FLASH_SECTOR_7_BANK1) && (Address >= ADDR_FLASH_SECTOR_6_BANK1)) || \
          ((Address < ADDR_FLASH_SECTOR_7_BANK2) && (Address >= ADDR_FLASH_SECTOR_6_BANK2)))    
  {
    sector = FLASH_SECTOR_6;  
  }
  else if(((Address < ADDR_FLASH_SECTOR_0_BANK2) && (Address >= ADDR_FLASH_SECTOR_7_BANK1)) || \
          ((Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_7_BANK2)))
  {
     sector = FLASH_SECTOR_7;  
  }
  else
  {
    sector = FLASH_SECTOR_7;
  }

  return sector;
}