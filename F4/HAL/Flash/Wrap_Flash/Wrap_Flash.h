/*!
 * @file      Wrap_Flash.h
 *
 * @brief     Wrap Flash module
 *
 * @author    Anosov Anton
 */

#ifndef WRAP_FLASH_H_
#define WRAP_FLASH_H_
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "Internal_Flash.h"
#include "Wrap_Flash_Cfg.h"
#include <stdbool.h>

/* Status storage */
#define FLASH_STORAGE_OK							(0x00)
#define FLASH_STORAGE_EXT_STRG_CORRUPTED			(0x01)
#define FLASH_STORAGE_EXT_STRG_WAS_EMPTY			(0x02)
#define FLASH_STORAGE_INT_STRG_WAS_EMPTY			(0x04)
#define FLASH_STORAGE_EXT_STRG_WRITE_ERR			(0x08)
#define FLASH_STORAGE_INT_STRG_WRITE_ERR			(0x10)
#define FLASH_STORAGE_UNEXP_BHVR_ERR				(0x20)

typedef uint8_t FlashStorageErr_t;

/*!
 * Flash storage description
 */
typedef struct FlashStorage_s
{
	/*!
	 * Flash map data description
	 */
	FlashMapData_t *pGlobalStorage;

	/*!
	 * Integrity of the data structure from external Flash
	 */
	uint8_t IsExtStorageOk:1;

	/*!
	 * Integrity of the data structure from internal Flash
	 */
	uint8_t IsIntStorageOk:1;

}FlashStorage_t;

/*!
 * @brief Function that performs the initialization of the storage
 *
 * @return FlashStorageErr_t - status operation
 */
FlashStorageErr_t FlashStorageInit(void);

/*!
 * @brief Function that writes the structure of the global store into memory
 *
 * @return FlashStorageErr_t - status operation
 */
FlashStorageErr_t GlobalStorageWrite(void);

/*!
 * @brief Reading the global storage
 *
 * @return Pointer to the FlashMapData_t
 */
FlashMapData_t* GlobalStorageRead(void);

#ifdef __cplusplus
}
#endif
#endif /* WRAP_FLASH_H_ */
