#include "eeprom.h"

#define DATA_64                 ((uint64_t)0x0123456780ABCDEF)

/*Variable used for Erase procedure*/
static FLASH_EraseInitTypeDef EraseInitStruct;

static uint32_t get_page(uint32_t Addr)
{
    return (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
}

int8_t eeprom_write(void * p_data, uint16_t size)
{
    uint8_t * ptr = (uint8_t *) p_data;

    if (size > FLASH_PAGE_SIZE)
    {
        return -1;
    }

#ifdef DEBUG_EEPROM
    printf("Size = %u\r\n", size);
#endif // DEBUG_EPROM

    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();

    /* Clear OPTVERR bit set on virgin samples */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

    /* Erase the user Flash area
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
    uint32_t first_page = 0, nb_of_pages = 0;
    uint32_t PageError = 0;
    /* Get the 1st page to erase */
    first_page = get_page(FLASH_USER_START_ADDR);

    /* Get the number of pages to erase from 1st page */
    nb_of_pages = get_page(FLASH_USER_END_ADDR) - first_page + 1;

    /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page        = first_page;
    EraseInitStruct.NbPages     = nb_of_pages;

    /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
        you have to make sure that these data are rewritten before they are accessed during code
        execution. If this cannot be done safely, it is recommended to flush the caches by setting the
        DCRST and ICRST bits in the FLASH_CR register. */
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
    {
        return -1;
    }
#ifdef DEBUG_EEPROM
    printf("WRITE\r\n");
#endif // DEBUG_EPROM

    for (uint16_t block_it = 0 ; (block_it * 8) < size ; block_it++)
    {
        uint64_t data_64 = 0;

        for (uint16_t data_it = 0 ; (data_it < 8) && (block_it*8 + data_it < size) ; data_it++)
        {
#ifdef DEBUG_EEPROM
            printf("%d:%x (%d)\r\n", block_it*8 + data_it, ptr[block_it*8 + data_it], (data_it*8));
#endif // DEBUG_EPROM

            data_64 += (uint64_t)(ptr[block_it*8 + data_it]) << (data_it*8);

#ifdef DEBUG_EEPROM
            printf("%d:0x%08x%08x\r\n", block_it*8 + data_it, (uint32_t)(data_64>>32), (uint32_t)data_64);
#endif // DEBUG_EPROM
        }

#ifdef DEBUG_EEPROM
        printf("FLASH[%d]:0x%08x%08x\r\n", block_it*8, (uint32_t)(data_64>>32), (uint32_t)data_64);
#endif // DEBUG_EPROM

        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_USER_START_ADDR + block_it*8, data_64) != HAL_OK)
        {
            return -1;
        }

#ifdef DEBUG_EEPROM
        printf("block_it*8 = %d ; size = %d\r\n", block_it*8, size);
#endif // DEBUG_EPROM        
    }

    /* Lock the Flash to disable the flash control register access (recommended
    to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();

    return 0;
}

int8_t eeprom_read(void * p_data, uint16_t size)
{
    uint8_t * ptr = (uint8_t * ) p_data;

    if (size > FLASH_PAGE_SIZE)
    {
        return -1;
    }

#ifdef DEBUG_EEPROM
    printf("READ\r\n");
#endif // DEBUG_EPROM  

	for (int i = 0 ; i < size ; i++) {
		ptr[i] = * (__IO uint8_t *)(FLASH_USER_START_ADDR+i);
#ifdef DEBUG_EEPROM
        printf("%d\r\n", ptr[i]);
#endif // DEBUG_EPROM  
	}

	return 0;
}
