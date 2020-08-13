/*!
 * @file      Wrap_Flash.c
 *
 * @brief     Wrap Flash module
 *
 * @author    Anosov Anton
 */

#include "Wrap_Flash.h"

#define FLASH_EXT_BUFF_SIZE			256
#define FLASH_STORAGE_INT_ADDRESS	0x08060000

uint8_t FlashExtBuff[FLASH_EXT_BUFF_SIZE];
FlashMapData_t GlobalStorage;
static FlashStorage_t StorageDescr = {&GlobalStorage, 1, 1};

//#define USE_EXTERNAL_FLASH

#ifdef USE_EXTERNAL_FLASH
/*!
 * @brief Write function to external storage
 *
 * @param pStorage  		Pointer to the FlashMapData_t
 * @return true - successful write, false - unsuccessful write
 */
static uint8_t WriteToExternalStorage(const FlashMapData_t *pStorage)
{
	CopyToExtStrgBuffer(pStorage);
	
	// IMPLEMENTATION
	// Writing and reading data structures from external Flash
	// See the implementation of WriteToInternalStorage

	if(CheckIntegritiesStructe((FlashMapData_t*) FlashExtBuff) != FLASH_STATUS_ERROR_CRC)
		return true;
	else
		return false;
}
#endif
/*!
 * @brief Write function to internal storage
 *
 * @param pStorage  		Pointer to the FlashMapData_t
 * @return true - successful write, false - unsuccessful write
 */
static uint8_t WriteToInternalStorage(const FlashMapData_t *pStorage)
{
	FlashMapData_t *pTmpStorage;

	FlashWriteStructe(FLASH_STORAGE_INT_ADDRESS, pStorage);
	pTmpStorage = (FlashMapData_t*) FlashReadStructe(FLASH_STORAGE_INT_ADDRESS);
	if (CheckIntegritiesStructe(pTmpStorage) != FLASH_STATUS_ERROR_CRC)
		return true;
	else
		return false;
}

/*!
 * @brief Function that checks the storage for empty space
 *
 * @param pStorage  		Pointer to the FlashMapData_t
 * @return true - storage is not empty, false - storage is empty
 */
static uint8_t IsStorageNotEmpty(const FlashMapData_t *pStorage)
{
	if(pStorage->Checksum != 0xFFFFFFFF)
		return true;
	else
		return false;
}

#ifdef USE_EXTERNAL_FLASH
/*!
 * @brief Function that copies data from the structure to the external Flash buffer
 *
 * @param pStorage  		Pointer to the FlashMapData_t
 */
static void CopyToExtStrgBuffer(const FlashMapData_t *pStorage)
{
	for(uint16_t i = 0; i < FLASH_STRUCT_USER_SIZE; i++)
	{
		FlashExtBuff[i] = *((uint8_t*)pStorage + i);
	}
}
#endif

/*!
 * @brief Function that fills the global storage by default
 *
 * @return true - the structure is filled in by default
 */
static uint8_t UseDefaultGlobalStorage(void)
{
	uint32_t crc = CRC_INI_VAL;

	// IMPLEMENTATION
	// Filling in the default structure with your own configs

	GlobalStorage.Checksum = ComputeChecksum(crc, (void*) StorageDescr.pGlobalStorage, FLASH_STRUCT_USER_SIZE - 4);

	return true;
}

#ifdef USE_EXTERNAL_FLASH
/*!
 * @brief Function for checking external flash
 *
 * @return true - external Flash is detected, false - external Flash was not detected
 */
static uint8_t CheckExternalFlash(void)
{
	// IMPLEMENTATION
	return false;
}
#endif

/*!
 * @brief Function that performs the initialization of the storage
 *
 * @return FlashStorageErr_t - status operation
 */
FlashStorageErr_t FlashStorageInit(void)
{
	FlashStorageErr_t ErrCode = FLASH_STORAGE_OK;
	FlashMapData_t* ExtStorage = NULL;
	FlashMapData_t* IntStorage = NULL;
	uint8_t IsStorageIntOk = 0, IsStorageExtOk = 0;

	IntStorage = (FlashMapData_t*) FlashReadStructe(FLASH_STORAGE_INT_ADDRESS);

#ifdef USE_EXTERNAL_FLASH
	if(CheckExternalFlash() != true)
	{
		// ERROR! Reading from an external flash is not possible!
		StorageDescr.IsExtStorageOk = 0;
		ErrCode |= FLASH_STORAGE_EXT_STRG_CORRUPTED;
	}
	else
	{
		// IMPLEMENTATION
		// Reading structure from external Flash to FlashExtBuff
		ExtStorage = (FlashMapData_t*) FlashExtBuff;
	}
#endif

	if (IsStorageNotEmpty(IntStorage))
	{
		if(CheckIntegritiesStructe(IntStorage) != FLASH_STATUS_ERROR_CRC)
		{
			// The structure in the internal Flash is not damaged
			IsStorageIntOk = 1; 
		}
	}
	else
	{
		ErrCode |= FLASH_STORAGE_INT_STRG_WAS_EMPTY;
	}

#ifdef USE_EXTERNAL_FLASH
	if (IsStorageNotEmpty(ExtStorage))
	{
		if(CheckIntegritiesStructe(ExtStorage) != FLASH_STATUS_ERROR_CRC)
		{
			// The structure in the external Flash is not damaged
			IsStorageExtOk = 1;
		}
	}
	else
	{
		ErrCode |= FLASH_STORAGE_EXT_STRG_WAS_EMPTY;
	}
#endif

    // State machine
	switch(ErrCode & (FLASH_STORAGE_EXT_STRG_WAS_EMPTY|FLASH_STORAGE_INT_STRG_WAS_EMPTY))
	{
		case FLASH_STORAGE_OK:
		{
#ifdef USE_EXTERNAL_FLASH
			if((IsStorageIntOk == 1) && (IsStorageExtOk == 1))
			{
				if (IntStorage->Checksum == ExtStorage->Checksum)
				{
					// Regular startup - data is correct in both stores
					GlobalStorage = *IntStorage;
				}
				else
				{
					// Storage is not damaged, copy from internal to external
					GlobalStorage = *IntStorage;
					if(WriteToExternalStorage(StorageDescr.pGlobalStorage) != 1)
					{
						ErrCode |= FLASH_STORAGE_EXT_STRG_WRITE_ERR;
					}
				}
			}
			else if ((IsStorageIntOk == 1) && (IsStorageExtOk != 1))
			{
				// External storage is damaged, copying from internal to external
				GlobalStorage = *IntStorage;
				if(WriteToExternalStorage(StorageDescr.pGlobalStorage) != 1)
				{
					ErrCode |= FLASH_STORAGE_EXT_STRG_WRITE_ERR;
				}
			}
			else if((IsStorageIntOk != 1) && (IsStorageExtOk == 1))	
			{			
				// Internal storage is damaged, copying from external to internal
				GlobalStorage = *ExtStorage;
				if(WriteToInternalStorage(StorageDescr.pGlobalStorage) != 1)
				{
					ErrCode |= FLASH_STORAGE_INT_STRG_WRITE_ERR;
				}
			}
			else
			{
				// Internal and external storage is damaged, we use default parameters
				UseDefaultGlobalStorage();
				if(WriteToInternalStorage(StorageDescr.pGlobalStorage) != 1)
				{
					ErrCode |= FLASH_STORAGE_INT_STRG_WRITE_ERR;
				}
				if(WriteToExternalStorage(StorageDescr.pGlobalStorage) != 1)
				{
					ErrCode |= FLASH_STORAGE_EXT_STRG_WRITE_ERR;
				}
			}
			break;
#else
			if(IsStorageIntOk == 1)
			{
				// Regular startup - data is correct in both stores
				GlobalStorage = *IntStorage;
			}
			else
			{
				// Internal storage is damaged, we use default parameters
				UseDefaultGlobalStorage();
				if(WriteToInternalStorage(StorageDescr.pGlobalStorage) != 1)
				{
					ErrCode |= FLASH_STORAGE_INT_STRG_WRITE_ERR;
				}
			}
			break;
#endif
		}
#ifdef USE_EXTERNAL_FLASH
		case FLASH_STORAGE_EXT_STRG_WAS_EMPTY:
		{
			if(IsStorageIntOk == 1)
			{
				// Internal storage is not damaged. Copying from internal to external
				GlobalStorage = *IntStorage;
				if(WriteToExternalStorage(StorageDescr.pGlobalStorage) != 1)
				{
					ErrCode |= FLASH_STORAGE_EXT_STRG_WRITE_ERR;
				}
			}
			else
			{
				// Internal and external storage is damaged, we use default parameters
				UseDefaultGlobalStorage();
				if(WriteToInternalStorage(StorageDescr.pGlobalStorage) != 1)
				{														
					ErrCode |= FLASH_STORAGE_INT_STRG_WRITE_ERR;
				}
				if(WriteToExternalStorage(StorageDescr.pGlobalStorage) != 1)
				{
					ErrCode |= FLASH_STORAGE_EXT_STRG_WRITE_ERR;
				}
			}
			break;
		}
#endif
		case FLASH_STORAGE_INT_STRG_WAS_EMPTY:
		{
#ifdef USE_EXTERNAL_FLASH
			if(IsStorageExtOk == 1)
			{
				// External storage is not damaged. Copying from external to internal
				GlobalStorage = *ExtStorage;
				if(WriteToInternalStorage(StorageDescr.pGlobalStorage) != 1)
				{
					ErrCode |= FLASH_STORAGE_INT_STRG_WRITE_ERR;
				}
			}
			else
			{
				// Internal and external storage is damaged, we use default parameters
				UseDefaultGlobalStorage();
				if(WriteToInternalStorage(StorageDescr.pGlobalStorage) != 1)
				{
					ErrCode |= FLASH_STORAGE_INT_STRG_WRITE_ERR;
				}
				if(WriteToExternalStorage(StorageDescr.pGlobalStorage) != 1)
				{
					ErrCode |= FLASH_STORAGE_EXT_STRG_WRITE_ERR;
				}
			}
			break;
#else
			// Internal storage is damaged, we use default parameters
			UseDefaultGlobalStorage();
			if(WriteToInternalStorage(StorageDescr.pGlobalStorage) != 1)
			{
				ErrCode |= FLASH_STORAGE_INT_STRG_WRITE_ERR;
			}
			break;
#endif
		}
#ifdef USE_EXTERNAL_FLASH
		case FLASH_STORAGE_EXT_STRG_WAS_EMPTY|FLASH_STORAGE_INT_STRG_WAS_EMPTY:
		{
			// Internal and external storage is damaged, we use default parameters
			UseDefaultGlobalStorage();
			if(WriteToInternalStorage(StorageDescr.pGlobalStorage) != 1)
			{
				ErrCode |= FLASH_STORAGE_INT_STRG_WRITE_ERR;
			}
			if(WriteToExternalStorage(StorageDescr.pGlobalStorage) != 1)
			{
				ErrCode |= FLASH_STORAGE_EXT_STRG_WRITE_ERR;
			}
			break;
		}
#endif
		default:
		{
			ErrCode = FLASH_STORAGE_UNEXP_BHVR_ERR;
			break;
		}
	}
	return ErrCode;
}

/*!
 * @brief Function that writes the structure of the global store into memory
 *
 * @return FlashStorageErr_t - status operation
 */
FlashStorageErr_t GlobalStorageWrite(void)
{
	FlashStorageErr_t ErrCode = FLASH_STORAGE_OK;
	uint32_t crc = CRC_INI_VAL;

	StorageDescr.pGlobalStorage->Checksum = ComputeChecksum(crc, (void*) StorageDescr.pGlobalStorage, FLASH_STRUCT_USER_SIZE - 4);

	if (WriteToInternalStorage(StorageDescr.pGlobalStorage) != 1)
	{
		ErrCode |= FLASH_STORAGE_INT_STRG_WRITE_ERR;
	}
#ifdef USE_EXTERNAL_FLASH
	if(StorageDescr.IsExtStorageOk != 1)
	{
		ErrCode |= FLASH_STORAGE_EXT_STRG_CORRUPTED;
	}
	else
	{
		if (WriteToExternalStorage(StorageDescr.pGlobalStorage) != 1)
		{
			ErrCode |= FLASH_STORAGE_EXT_STRG_WRITE_ERR;
		}
	}
#endif
	return ErrCode;
}

/*!
 * @brief Reading the global storage
 *
 * @return Pointer to the FlashMapData_t
 */
FlashMapData_t* GlobalStorageRead(void)
{
	return StorageDescr.pGlobalStorage;
}
