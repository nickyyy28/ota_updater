#include "boot.h"

APP_FUNC app_func = 0;

__asm void MSR_MSP(uint32_t addr);

void boot_to(uint32_t addr)
{
    if (addr & 0x2FFF0000 == 0x20000000) {
        app_func = (APP_FUNC)(addr + 4);
        MSR_MSP(addr);
        app_func();
    }
}

__asm void MSR_MSP(uint32_t addr)
{
    MSR MSP, r0
    BX lr
}