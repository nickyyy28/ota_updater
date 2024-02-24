#include "version.h"

#ifdef USE_APP_AERA1

__attribute__((at(0x08040000))) const Firmware_Version_Typedef firmware_info = {
	.device_name = "STM32H7 OTA Demo",
	.Version = VERSION_TO_UINT32(1, 1, 0),
	.update_timestamp = 1708777863,
	.manufacturer = "Nickyyy",
	.description = "This is a simple ota demo",
	.size = 67612,
	.crc16 = 0
};



#endif