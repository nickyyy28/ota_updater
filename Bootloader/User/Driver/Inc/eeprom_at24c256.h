#ifndef __EEPROM_AT24C256_H
#define __EEPROM_AT24C256_H

#include "stdint.h"
#include "uni_shell.h"

void eeprom_read_page(uint8_t device_addr, uint16_t page_addr, uint8_t in_page_addr, uint8_t *buffer, uint32_t len);
void eeprom_write_str(uint8_t device_addr, uint8_t write_addr, uint8_t *buffer, uint32_t len);

#endif //__EEPROM_AT24C256_H
