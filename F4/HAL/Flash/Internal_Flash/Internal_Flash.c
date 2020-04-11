/*!
 * \file      Internal_Flash.c
 *
 * \brief     Internal flash module
 *
 * \author    Anosov Anton
 */

#include "Internal_Flash.h"

static uint8_t FlashMapData_s[FLASH_STRUCT_USER_SIZE];

const uint32_t CRC32_Table[256] = {
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};

/*!
 * \brief Get the sector number at the address
 *
 * \param[IN] Address  		Base address
 * \retval           		Get number sector
 */
static uint32_t GetNumSector(uint32_t Address)
{
	uint32_t Sector = 0;

	if(Address >= START_ADDRESS_SECTOR_0 && Address < START_ADDRESS_SECTOR_1)
		Sector = FLASH_SECTOR_0;
	else if(Address >= START_ADDRESS_SECTOR_1 && Address < START_ADDRESS_SECTOR_2)
		Sector = FLASH_SECTOR_1;
	else if(Address >= START_ADDRESS_SECTOR_2 && Address < START_ADDRESS_SECTOR_3)
		Sector = FLASH_SECTOR_2;
	else if(Address >= START_ADDRESS_SECTOR_3 && Address < START_ADDRESS_SECTOR_4)
		Sector = FLASH_SECTOR_3;
	else if(Address >= START_ADDRESS_SECTOR_4 && Address < START_ADDRESS_SECTOR_5)
		Sector = FLASH_SECTOR_4;
	else if(Address >= START_ADDRESS_SECTOR_5 && Address < START_ADDRESS_SECTOR_6)
		Sector = FLASH_SECTOR_5;
	else if(Address >= START_ADDRESS_SECTOR_6 && Address < START_ADDRESS_SECTOR_7)
		Sector = FLASH_SECTOR_6;
	else if(Address >= START_ADDRESS_SECTOR_7 && Address < END_ADDRESS_BANK_1)
		Sector = FLASH_SECTOR_7;

	return Sector;
}

/*!
 * \brief Initialization flash
 *
 * \retval           		Status of the operation
 */
FlashStatus_t FlashInit(void)
{
	FlashStatus_t ErrCode = FLASH_STATUS_OK;
	uint32_t CoreHCKL;

	if(HAL_FLASH_Unlock() != HAL_OK)
	{
		ErrCode = FLASH_STATUS_ERROR_UNLOCK;
		return ErrCode;
	}

	CoreHCKL = HAL_RCC_GetHCLKFreq();

	if(CoreHCKL > 0 && CoreHCKL <= 30000000)
		__HAL_FLASH_SET_LATENCY(FLASH_LATENCY_0);
	else if(CoreHCKL > 30000000 && CoreHCKL <= 60000000)
		__HAL_FLASH_SET_LATENCY(FLASH_LATENCY_1);
	else if(CoreHCKL > 60000000 && CoreHCKL <= 90000000)
		__HAL_FLASH_SET_LATENCY(FLASH_LATENCY_2);
	else if(CoreHCKL > 90000000 && CoreHCKL <= 120000000)
		__HAL_FLASH_SET_LATENCY(FLASH_LATENCY_3);
	else if(CoreHCKL > 120000000 && CoreHCKL <= 150000000)
		__HAL_FLASH_SET_LATENCY(FLASH_LATENCY_4);
	else if(CoreHCKL > 150000000 && CoreHCKL <= 180000000)
		__HAL_FLASH_SET_LATENCY(FLASH_LATENCY_5);

	__HAL_FLASH_PREFETCH_BUFFER_ENABLE();

	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
			FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

	if(HAL_FLASH_Lock() != HAL_OK)
	{
		ErrCode = FLASH_STATUS_ERROR_LOCK;
		return ErrCode;
	}

	return ErrCode;
}

/*!
 * \brief Reads data at the specified address
 *
 * \param[IN] Address  		Base address
 * \retval           		Data
 */
uint32_t FlashReadData(uint32_t Address)
{
	return (*(__IO uint32_t *)Address);
}

/*!
 * \brief Reads structe data at the specified address
 *
 * \param[IN] Address  		Base address
 * \retval           		Data structe pointer
 */
FlashMapData_t* FlashReadStructe(uint32_t Address)
{
	uint32_t *pData = (uint32_t*)FlashMapData_s;

	for(uint32_t i = 0; i < FLASH_STRUCT_USER_SIZE; i += 4)
	{
		*pData = FlashReadData(Address + i);
		pData++;
	}

	return (FlashMapData_t*)FlashMapData_s;
}

/*!
 * \brief Erase sector flash
 *
 * \param[IN] Address  		Base address
 * \retval           		Status of the operation
 */
FlashStatus_t FlashEraseSector(uint32_t Address)
{
	FlashStatus_t ErrCode = FLASH_STATUS_OK;
	FLASH_EraseInitTypeDef FlashErase_s;
	uint32_t SectorError;

	if(HAL_FLASH_Unlock() != HAL_OK)
	{
		ErrCode = FLASH_STATUS_ERROR_UNLOCK;
		return ErrCode;
	}

	FlashErase_s.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	FlashErase_s.Sector = GetNumSector(Address);
	FlashErase_s.NbSectors = 1;
	FlashErase_s.TypeErase = FLASH_TYPEERASE_SECTORS;

	while(FLASH_WaitForLastOperation((uint32_t)MAX_FLASH_TIMEOUT) != HAL_OK);

	if(HAL_FLASHEx_Erase(&FlashErase_s, &SectorError) != HAL_OK)
	{
		ErrCode = FLASH_STATUS_ERROR_ERASE;
		return ErrCode;
	}

	if(HAL_FLASH_Lock() != HAL_OK)
	{
		ErrCode = FLASH_STATUS_ERROR_LOCK;
		return ErrCode;
	}

	return ErrCode;
}

/*!
 * \brief Write data to flash
 *
 * \param[IN] Address  		Base address
 * \param[IN] pData  		Data pointer
 * \param[IN] Size  		Data size
 * \retval           		Status of the operation
 */
FlashStatus_t FlashWriteData(uint32_t Address, uint32_t *pData, uint32_t Size)
{
	FlashStatus_t ErrCode = FLASH_STATUS_OK;

	if((ErrCode = FlashEraseSector(Address)) != FLASH_STATUS_OK)
		return ErrCode;

	if(HAL_FLASH_Unlock() != HAL_OK)
	{
		ErrCode = FLASH_STATUS_ERROR_UNLOCK;
		return ErrCode;
	}

	for(uint32_t i = 0; i < Size; i += 4)
	{
		if(HAL_FLASH_Program(TYPEPROGRAM_WORD, Address + i, *(pData)) != HAL_OK)
		{
			ErrCode = FLASH_STATUS_ERROR_WRITE;
			return ErrCode;
		}
		pData++;
	}

	if(HAL_FLASH_Lock() != HAL_OK)
	{
		ErrCode = FLASH_STATUS_ERROR_LOCK;
		return ErrCode;
	}

	return ErrCode;
}

/*!
 * \brief Writes structe data at the specified address
 *
 * \param[IN] Address  		Base address
 * \param[IN] pStruct  		Data structe pointer
 * \retval           		Status of the operation
 */
FlashStatus_t FlashWriteStructe(uint32_t Address, FlashMapData_t *pStruct)
{
	FlashStatus_t ErrCode = FLASH_STATUS_OK;
	uint32_t *pData = (uint32_t*)pStruct;

	if((ErrCode = FlashEraseSector(Address)) != FLASH_STATUS_OK)
		return ErrCode;

	if(HAL_FLASH_Unlock() != HAL_OK)
	{
		ErrCode = FLASH_STATUS_ERROR_UNLOCK;
		return ErrCode;
	}

	pStruct->Checksum = ComputeChecksum((uint32_t)CRC_INI_VAL, pStruct, FLASH_STRUCT_USER_SIZE - 4);

	for(uint32_t i = 0; i < FLASH_STRUCT_USER_SIZE; i += 4)
	{
		if(HAL_FLASH_Program(TYPEPROGRAM_WORD, Address + i, *(pData)) != HAL_OK)
		{
			ErrCode = FLASH_STATUS_ERROR_WRITE;
			return ErrCode;
		}
		pData++;
	}

	if(HAL_FLASH_Lock() != HAL_OK)
	{
		ErrCode = FLASH_STATUS_ERROR_LOCK;
		return ErrCode;
	}

	return ErrCode;
}

/*!
 * \brief Calculating the checksum (CRC32)
 * using the polynomial x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x + 1
 *
 * \param[IN] Crc  			Default crc ini val
 * \param[IN] pData  		Data pointer
 * \param[IN] Size  		Data size
 * \retval           		The value of the checksum
 */
uint32_t ComputeChecksum(uint32_t Crc, void* pData, uint32_t Size)
{
	while (Size > 0)
	{
		Crc = CRC32_NEXT(Crc, *(uint8_t*)pData);
		pData = (uint8_t*)pData + 1;
		Size--;
	}

	return Crc;
}

/*!
 * \brief Check integrities structe with use checksum
 *
 * \param[IN] pStruct  		Data structe pointer
 * \retval           		Status of the operation
 */
FlashStatus_t CheckIntegritiesStructe(FlashMapData_t *pStruct)
{
	FlashStatus_t ErrCode = FLASH_STATUS_OK;
	uint32_t Crc = CRC_INI_VAL;

	if((Crc = ComputeChecksum((uint32_t)CRC_INI_VAL, (void*) pStruct, FLASH_STRUCT_USER_SIZE)))
		ErrCode = FLASH_STATUS_ERROR_CRC;

	return ErrCode;
}
