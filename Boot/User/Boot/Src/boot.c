#include "boot.h"
#include "cmsis_armcc.h"

volatile APP_FUNC app_func = 0;
volatile uint32_t app_address = 0;
volatile uint32_t jump_address = 0;

__asm void MSR_MSP(uint32_t addr);

volatile uint32_t tmp_data = 0;

void boot_to(uint32_t addr)
{
	app_address = addr;
	tmp_data = (*(volatile uint32_t *)app_address);
	//if (((*(volatile uint32_t *)app_address) & 0x2FFE0000) == 0x20000000) {
		jump_address = *(volatile uint32_t*)(app_address + 4U);
		app_func = (APP_FUNC)jump_address;
		__set_MSP(*(volatile uint32_t*)app_address);
		app_func();
	//}
    /*if (addr & 0x2FFF0000 == 0x20000000) {
        app_func = (APP_FUNC)(addr + 4);
        MSR_MSP(addr);
        app_func();
    }*/
	
	
}

__asm void MSR_MSP(uint32_t addr)
{
    MSR MSP, r0
    BX lr
}