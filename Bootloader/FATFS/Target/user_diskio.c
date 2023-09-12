/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * @file    user_diskio.c
  * @brief   This file includes a diskio driver skeleton to be completed by the user.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
 /* USER CODE END Header */

#ifdef USE_OBSOLETE_USER_CODE_SECTION_0
/*
 * Warning: the user section 0 is no more in use (starting from CubeMx version 4.16.0)
 * To be suppressed in the future.
 * Kept to ensure backward compatibility with previous CubeMx versions when
 * migrating projects.
 * User code previously added there should be copied in the new user sections before
 * the section contents can be deleted.
 */
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */
#endif

/* USER CODE BEGIN DECL */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "ff_gen_drv.h"
#include "w25qxx_4M.h"
#include "uni_shell.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define PAGE_SIZE       256
#define SECTOR_SIZE     4096
#define SECTOR_COUNT	2048
#define BLOCK_SIZE		65536
#define BLOCK_COUNT		128
#define FLASH_PAGES_PER_SECTOR	SECTOR_SIZE/PAGE_SIZE
/* Private variables ---------------------------------------------------------*/
/* Disk status */
FATFS fs;
FIL file;						  	/* 文件对象 */
FRESULT f_res;                    	/* 文件操作结果 */
UINT fnum;            				/* 文件成功读写数量 */
BYTE ReadBuffer[1024]={0};        	/* 读缓冲区 */
BYTE WriteBuffer[]= "WWB is the high hand\n";
BYTE work[4096];
UINT bw;
char USER_Path[4];

static volatile DSTATUS Stat = STA_NOINIT;

void FileTest(void)
{
    FRESULT res;  /* API result code */
	BYTE mm[50];
	UINT i;
	LOG_DEBUG("文件系统测试�?�?.");
	/* 挂载文件系统 */
	res = f_mount(&fs, "0:", 0);
	if (res)
	{
		LOG_DEBUG("文件系统挂载失败.");
	}
	else
	{
		LOG_DEBUG("文件系统挂载成功.");
	}
	/* Create a file as new */
	/*res = f_open(&file, "0:/test.txt", FA_CREATE_NEW|FA_WRITE|FA_READ);
	if (res)
	{
		SHELL_DEBUG("打开文件失败.");
	}
	else
	{
		SHELL_DEBUG("打开文件成功.");
	}
	res = f_write(&file, "Hello,World!", 12, &bw);
	//uart_printf("res write:%d\r\n",res);
	if (bw == 12)
	{
		SHELL_DEBUG("写文件成�?!");
	}
	else
	{
		SHELL_DEBUG("写文件失�?!");
	}
	res = f_size(&file);
	SHELL_DEBUG("文件大小:%d Bytes.",res);
	memset(mm,0x0,50);
	
	//f_lseek(&file,0);
	res = f_close(&file);
	if (res) {
		SHELL_DEBUG("文件保存失败.");
	} else {
		SHELL_DEBUG("文件保存成功.");
	}*/
	
	res = f_open(&file, "0:/test.txt", FA_READ);
	if (res) {
		LOG_DEBUG("打开文件失败.");
		return;
	} else {
		LOG_DEBUG("打开文件成功.");
	}
	res = f_read(&file,mm,12,&i);
	if (res == FR_OK)
	{
		LOG_DEBUG("读文件成�?!");
		LOG_DEBUG("读到数据长度:%d Bytes.",i);
	}
	else
	{
		LOG_DEBUG("读文件失�?!");
	}
	LOG_DEBUG("读到如下数据:%s", mm);
	//buff_print((char *)mm,12);
	/* Close the file */
	f_close(&file);
	/*卸载文件系统*/
	f_mount(0, "0:", 0);
	LOG_DEBUG("文件系统测试完毕.");
} 

/* USER CODE END DECL */

/* Private function prototypes -----------------------------------------------*/
DSTATUS USER_initialize (BYTE pdrv);
DSTATUS USER_status (BYTE pdrv);
DRESULT USER_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count);
#if _USE_WRITE == 1
  DRESULT USER_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
  DRESULT USER_ioctl (BYTE pdrv, BYTE cmd, void *buff);
#endif /* _USE_IOCTL == 1 */

Diskio_drvTypeDef  USER_Driver =
{
  USER_initialize,
  USER_status,
  USER_read,
#if  _USE_WRITE
  USER_write,
#endif  /* _USE_WRITE == 1 */
#if  _USE_IOCTL == 1
  USER_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes a Drive
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS USER_initialize (
	BYTE pdrv           /* Physical drive nmuber to identify the drive */
)
{
  /* USER CODE BEGIN INIT */
    //Stat = W25Qx_Init();
    // Stat = STA_NOINIT;
	Stat = STA_NOINIT;
	if((W25qxx_ReadID() & 0x000000FF) != 0)
	{
		//Stat &= ~STA_NOINIT;
		return RES_OK;
	}	
    return RES_OK;
  /* USER CODE END INIT */
}

/**
  * @brief  Gets Disk Status
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS USER_status (
	BYTE pdrv       /* Physical drive number to identify the drive */
)
{
  /* USER CODE BEGIN STATUS */
    Stat &= ~STA_NOINIT;
    return RES_OK;
  /* USER CODE END STATUS */
}

/**
  * @brief  Reads Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT USER_read (
	BYTE pdrv,      /* Physical drive nmuber to identify the drive */
	BYTE *buff,     /* Data buffer to store read data */
	DWORD sector,   /* Sector address in LBA */
	UINT count      /* Number of sectors to read */
)
{
  /* USER CODE BEGIN READ */
  /*SHELL_DEBUG("FLASH READ sector addr: %d, count: %d", sector, count);
    for (uint32_t i = 0 ; i < count ; i++ ) {
      SHELL_DEBUG("current count: %d", i);
      SHELL_DEBUG("W25Qx read addr %d, size: %d", sector * W25Qx_Para.SUBSECTOR_SIZE, W25Qx_Para.SUBSECTOR_SIZE);
      W25Qx_Read(buff, sector * W25Qx_Para.SUBSECTOR_SIZE, W25Qx_Para.SUBSECTOR_SIZE);
			SHELL_DEBUG("aaa");
      buff += W25Qx_Para.SUBSECTOR_SIZE; 
      sector ++;
      
    }
		SHELL_DEBUG("READ Complete");
    return RES_OK;*/
	DRESULT res = RES_ERROR;
	UINT i;
  
	for(i = 0;i < count;i++)
	{
		//sFLASH_ReadBuffer(buff + i * 4096,sector * 4096 + i * 4096,4096 );
		LOG_DEBUG("W25qxx_ReadSector");
		W25qxx_ReadSector(buff + i * 4096, sector + i, 0, 4096);
	}
  /* USER CODE END READ */
}

/**
  * @brief  Writes Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if _USE_WRITE == 1
DRESULT USER_write (
	BYTE pdrv,          /* Physical drive nmuber to identify the drive */
	const BYTE *buff,   /* Data to be written */
	DWORD sector,       /* Sector address in LBA */
	UINT count          /* Number of sectors to write */
)
{
  /* USER CODE BEGIN WRITE */
	/*DRESULT ret;
  for (uint32_t i = 0 ; i < count ; i++ ) {
    W25Qx_Erase_Block(sector * W25Qx_Para.SUBSECTOR_SIZE);
    ret |= W25Qx_Write(buff, sector * W25Qx_Para.SUBSECTOR_SIZE, W25Qx_Para.SUBSECTOR_SIZE);
    buff += W25Qx_Para.SUBSECTOR_SIZE;
    sector ++;
  }*/
  /* USER CODE HERE */
	DRESULT res = RES_ERROR;
	
	UINT i;
	
	//SHELL_DEBUG("flash write sector count = %d", count);
  
	for(i = 0;i < count;i++)
	{
		//SHELL_DEBUG("writing sector:%d", sector + i);
		W25qxx_EraseSector(sector + i);
		W25qxx_WriteSector((uint8_t *)(buff + SECTOR_SIZE * i), sector + i, 0, SECTOR_SIZE);
	}
	
	
	res = RES_OK;
    return res;
  /* USER CODE END WRITE */
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  I/O control operation
  * @param  pdrv: Physical drive number (0..)
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL == 1
DRESULT USER_ioctl (
	BYTE pdrv,      /* Physical drive nmuber (0..) */
	BYTE cmd,       /* Control code */
	void *buff      /* Buffer to send/receive control data */
)
{
  /* USER CODE BEGIN IOCTL */
    DRESULT res = RES_ERROR;
    return res;
  /* USER CODE END IOCTL */
}
#endif /* _USE_IOCTL == 1 */

