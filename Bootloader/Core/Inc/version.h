#include <stdint.h>
 
#define VERSION_TO_UINT32(major, minor, patch)   \
    ((uint32_t)(((uint32_t)major << 24) | ((uint32_t)minor << 16) | (uint32_t)patch))

#define Get_Version_Major(version)	\
	((uint32_t)(version >> 24) & 0xFF)
	
#define Get_Version_Minor(version)	\
	((uint32_t)(version >> 16) & 0xFF)
	
#define Get_Version_Patch(version)	\
	((uint32_t)(version) & 0xFFFF)

typedef __packed struct{
	char device_name[20];
	uint32_t Version;
	uint32_t update_timestamp;
	char manufacturer[20];
	char description[30];
	uint32_t size;
	uint16_t crc16;    
}Firmware_Version_Typedef;
