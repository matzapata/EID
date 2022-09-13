#ifndef FLASH_H_
#define FLASH_H_

/********************************************************************************************
 * INCLUDES
 ********************************************************************************************/
#include "fsl_iap.h"
#include "fsl_common.h"

/********************************************************************************************
 * CONSTANTES
 ********************************************************************************************/
#define PAGE_MAX_SIZE 64
#define PAGES_PER_SECTOR 16

/********************************************************************************************
 * PROTOTIPOS FUNCIONES PUBLICAS
 ********************************************************************************************/
void Flash_Init(void);
uint8_t Flash_WritePage(uint32_t sector, uint32_t page, uint32_t * buffer, uint32_t size);
uint32_t Flash_GetPageAddress(uint32_t sector, uint32_t page);
uint8_t * Flash_GetPageMemCpy(uint32_t sector, uint32_t page);


#endif /* FLASH_H_ */
