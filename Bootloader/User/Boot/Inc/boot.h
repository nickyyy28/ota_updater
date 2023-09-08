#ifndef BOOTLOADER_H

#include <stdint.h>

typedef void (*APP_FUNC)(void);

void boot_to(uint32_t addr);

#endif // !BOOTLOADER_H