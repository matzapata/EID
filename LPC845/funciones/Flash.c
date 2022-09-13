#include "Flash.h"

#include <stdio.h>

static uint32_t s_PageBuf[FSL_FEATURE_SYSCON_FLASH_PAGE_SIZE_BYTES / sizeof(uint32_t)];
static uint32_t s_IapFlashPageNum =
		FSL_FEATURE_SYSCON_FLASH_SECTOR_SIZE_BYTES / FSL_FEATURE_SYSCON_FLASH_PAGE_SIZE_BYTES;


void Flash_Init(void) {
	IAP_ConfigAccessFlashTime(kFlash_IAP_OneSystemClockTime);
}

uint8_t Flash_WritePage(uint32_t sector, uint32_t page, uint32_t * buffer, uint32_t size) {
	if (size > FSL_FEATURE_SYSCON_FLASH_PAGE_SIZE_BYTES) return 0;

	status_t status;
	uint32_t s_IapFlashPage = (FSL_FEATURE_SYSCON_FLASH_SECTOR_SIZE_BYTES / FSL_FEATURE_SYSCON_FLASH_PAGE_SIZE_BYTES) * sector;
	uint32_t dstAddr = (s_IapFlashPage + page) * FSL_FEATURE_SYSCON_FLASH_PAGE_SIZE_BYTES;

	memset(s_PageBuf, '\0', FSL_FEATURE_SYSCON_FLASH_PAGE_SIZE_BYTES);
	memcpy(s_PageBuf, buffer, size);

	printf("Escribimos: %s en: %p\n", s_PageBuf, dstAddr);

	/* Borramos pagina */
	IAP_PrepareSectorForWrite(sector, sector);
	IAP_ErasePage(s_IapFlashPage + page, s_IapFlashPage + page, SystemCoreClock);

	/* Escribimos */
	IAP_PrepareSectorForWrite(sector, sector);
	status = IAP_CopyRamToFlash(
			dstAddr,
			&s_PageBuf[0],
			FSL_FEATURE_SYSCON_FLASH_PAGE_SIZE_BYTES,
			SystemCoreClock);
	if (status != kStatus_IAP_Success)
	{
		printf("ERROR: Escritura pagina");
		return 0;
	}

	return 1;
}


uint32_t Flash_GetPageAddress(uint32_t sector, uint32_t page) {
	uint32_t s_IapFlashPage = (FSL_FEATURE_SYSCON_FLASH_SECTOR_SIZE_BYTES / FSL_FEATURE_SYSCON_FLASH_PAGE_SIZE_BYTES) * sector;
	uint32_t dstAddr = (s_IapFlashPage + page) * FSL_FEATURE_SYSCON_FLASH_PAGE_SIZE_BYTES;
	return dstAddr;
}

uint8_t * Flash_GetPageMemCpy(uint32_t sector, uint32_t page) {
	uint32_t s_IapFlashPage = (FSL_FEATURE_SYSCON_FLASH_SECTOR_SIZE_BYTES / FSL_FEATURE_SYSCON_FLASH_PAGE_SIZE_BYTES) * sector;
	uint32_t srcAddr = (s_IapFlashPage + page) * FSL_FEATURE_SYSCON_FLASH_PAGE_SIZE_BYTES;

	uint8_t * PageCpy = (uint8_t *) malloc(FSL_FEATURE_SYSCON_FLASH_PAGE_SIZE_BYTES);

	memcpy(PageCpy, srcAddr, FSL_FEATURE_SYSCON_FLASH_PAGE_SIZE_BYTES);

	return PageCpy;
}
