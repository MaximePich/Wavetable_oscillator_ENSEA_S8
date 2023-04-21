#ifndef __DRV_EEPROM_H
#define __DRV_EEPROM_H

#define ADDR_FLASH_PAGE_63    ((uint32_t)0x0801F800) /* Base @ of Page 63, 2 Kbytes */

#define FLASH_USER_START_ADDR   ADDR_FLASH_PAGE_63   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     (ADDR_FLASH_PAGE_63 + FLASH_PAGE_SIZE - 1)   /* End @ of user Flash area */

#include "stm32g4xx_hal.h"

int8_t eeprom_write(void * p_data, uint16_t size);
int8_t eeprom_read(void * p_data, uint16_t size);

#endif //__DRV_EEPROM_H