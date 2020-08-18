/*!
 * @file      AT45DB.c
 *
 * @brief     External flash AT45DB implementation
 *
 * @author    Anosov Anton
 */

#include "AT45DB.h"

//#define MEMORY_BOUNDARY_CHECKS
#define AT45DB_SIZE_BUFFER	256
#define AT45DB_TIMEOUT		100

unsigned char AT45BD_IOBuff[AT45DB_SIZE_BUFFER];

/*!
 * @brief Function that reads / writes Flash memory over the SPI interface
 *
 * @param OpCode			Pointer to the command code
 * @param OpCodeLen			Length of the command code
 * @param IOBuff			Pointer to an array of received / sent data
 * @param IOBuffSize		Size of received / sent data
 * @param Operation			Operation type (read / write)
 */
static void AT45DB_Transfer(uint8_t *OpCode, uint8_t OpCodeLen, uint8_t *IOBuff, uint16_t IOBuffSize,
							AT45DB_Operation_t Operation)
{
	if(Operation == AT45DB041_READ)
	{
		AT45DB_CS_LOW();
		HAL_SPI_Transmit(&AT45DB_SPI, OpCode, OpCodeLen, AT45DB_TIMEOUT);
		HAL_SPI_Receive(&AT45DB_SPI, IOBuff, IOBuffSize, AT45DB_TIMEOUT);
		AT45DB_CS_HIGH();
	}
	else if(Operation == AT45DB041_WRITE)
	{
		AT45DB_CS_LOW();
		HAL_SPI_Transmit(&AT45DB_SPI, OpCode, OpCodeLen, AT45DB_TIMEOUT);
		if(IOBuffSize)
		{
			HAL_SPI_Transmit(&AT45DB_SPI, IOBuff, IOBuffSize, AT45DB_TIMEOUT);
		}
		AT45DB_CS_HIGH();
	}
}

/*!
 * @brief Function that waits for the end of internal Flash memory chip operations
 */
static void AT45DB_WaitForFlash(void)
{
    uint8_t Status = true;
    uint8_t Reg[2] = {FLASH_OPCODE_READ_STAT_REG, 0};

    while(Status == true)
    {
    	AT45DB_Transfer(Reg, 1, Reg + 1, 1, AT45DB041_READ);
        if(Reg[1]&FLASH_READY_BITMASK)
        	Status = false;
    }
}

static inline void AT45DB_FillPageAddress(uint16_t PageNumber, uint16_t Offset, uint8_t* pBuffer)
{
#if (READ_OFFSET_SHT_AMT != WRITE_SHT_AMT) || (READ_OFFSET_SHT_AMT != PAGE_ERASE_SHT_AMT)
	#error "read / write / erase bitshifts differ"
#endif
    uint16_t Temp = (PageNumber << (READ_OFFSET_SHT_AMT - 8)) | (Offset >> 8);
    pBuffer[0] = (uint8_t)((Temp >> 8) & 0xFF);
    pBuffer[1] = (uint8_t)(Temp & 0xFF);
    pBuffer[2] = (uint8_t)(Offset & 0xFF);
}

void AT45DB_MemoryBoundaryErrorCallback(void)
{
	// We'll add more debug later if needed
#ifdef USB_MESSAGES_FOR_CRITICAL_CALLBACKS
        usbPutstr("#MBE");
#endif
    while(1);
}

/*!
 * @brief Function that reads Flash memory chip IDs (Manufacture ID, Device Id (2 bytes), EDI (2 bytes))
 *
 * @return true - successful reading, false - unsuccessful reading
 */
uint8_t AT45DB_CheckID(void)
{
    uint8_t Buff[6];

    Buff[0] = FLASH_OPCODE_READ_DEV_INFO;

    // Read Manufacturer ID, Device ID (2 bytes), EDI (2 bytes)
    AT45DB_Transfer(Buff, 1, Buff + 1, 5, AT45DB041_READ);

    if((Buff[1] != FLASH_MANUF_ID) || (Buff[2] != MAN_FAM_DEN_VAL))
    	return false;
    else
        return true;
}

/*!
 * @brief Function that performs a complete cleaning of the Flash memory chip
 */
void AT45DB_ChipErase(void)
{
    uint8_t Buff[4] = {0xC7, 0x94, 0x80, 0x9A};

    AT45DB_Transfer(Buff, 4, 0, 0, AT45DB041_WRITE);
    AT45DB_WaitForFlash();
}

/*!
 * @brief Function that clears the sector of the Flash memory chip (Sector Erase 64 KB)
 *
 * @param SectorNumber		Number sector (Sector 0a, Sector 0b)
 */
void AT45DB_SectorZeroErase(uint8_t SectorNumber)
{
    uint8_t Buff[4];
    uint16_t Temp = (uint16_t)SectorNumber << (SECTOR_ERASE_0_SHT_AMT - 8);

#ifdef MEMORY_BOUNDARY_CHECKS
    // Error check parameter sectorNumber
    if(!(SectorNumber == FLASH_SECTOR_ZERO_A_CODE || SectorNumber == FLASH_SECTOR_ZERO_B_CODE))
    {
    	AT45DB_MemoryBoundaryErrorCallback();
    }
#endif

    Buff[0] = FLASH_OPCODE_SECTOR_ERASE;
    Buff[1] = (uint8_t)((Temp >> 8) & 0xFF);
    Buff[2] = (uint8_t)(Temp & 0xFF);
    Buff[3] = 0;
    AT45DB_Transfer(Buff, 1, Buff + 1, 3, AT45DB041_WRITE);
    AT45DB_WaitForFlash();
}

/*!
 * @brief Function that clears the sector of the Flash memory chip (Sector Erase 64 KB)
 *
 * @param SectorNumber		Number sector
 */
void AT45DB_SectorErase(uint8_t SectorNumber)
{
    uint8_t Buff[4];
    uint16_t Temp = (uint16_t)SectorNumber << (SECTOR_ERASE_N_SHT_AMT - 8);

#ifdef MEMORY_BOUNDARY_CHECKS
    // Error check parameter sectorNumber
    if((SectorNumber < SECTOR_START) || (SectorNumber > SECTOR_END)) // Ex: 1M -> SECTOR_START = 1, SECTOR_END = 3  sectorNumber must be 1, 2, or 3
    {
    	AT45DB_MemoryBoundaryErrorCallback();
    }
#endif

    Buff[0] = FLASH_OPCODE_SECTOR_ERASE;
    Buff[1] = (uint8_t)((Temp >> 8) & 0xFF);
    Buff[2] = (uint8_t)(Temp & 0xFF);
    Buff[3] = 0;
    AT45DB_Transfer(Buff, 1, Buff + 1, 3, AT45DB041_WRITE);
    AT45DB_WaitForFlash();
}

/*!
 * @brief Function that clears a single block of Flash memory chip (Block Erase 2 KB)
 *
 * @param BlockNumber		Number of the block
 */
void AT45DB_BlockErase(uint16_t BlockNumber)
{
    uint8_t Buff[4];
    uint16_t Temp = BlockNumber << (BLOCK_ERASE_SHT_AMT - 8);

#ifdef MEMORY_BOUNDARY_CHECKS
    // Error check parameter blockNumber
    if(BlockNumber >= BLOCK_COUNT)// Ex: 1M -> BLOCK_COUNT = 64.. valid pageNumber 0-63
    {
    	AT45DB_MemoryBoundaryErrorCallback();
    }
#endif

    Buff[0] = FLASH_OPCODE_BLOCK_ERASE;
    Buff[1] = (uint8_t)((Temp >> 8) & 0xFF);
    Buff[2] = (uint8_t)(Temp & 0xFF);
    Buff[3] = 0;
    AT45DB_Transfer(Buff, 1, Buff + 1, 3, AT45DB041_WRITE);
    AT45DB_WaitForFlash();
}

/*!
 * @brief Function that clears a single page of the Flash memory chip (Page Erase 256 bytes)
 *
 * @param PageNumber		Number of the page
 */
void AT45DB_PageErase(uint16_t PageNumber)
{
    uint8_t Buff[4];

#ifdef MEMORY_BOUNDARY_CHECKS
    // Error check parameter pageNumber
    if(PageNumber >= PAGE_COUNT) // Ex: 1M -> PAGE_COUNT = 512.. valid pageNumber 0-511
    {
    	AT45DB_MemoryBoundaryErrorCallback();
    }
#endif

    Buff[0] = FLASH_OPCODE_PAGE_ERASE;
    AT45DB_FillPageAddress(PageNumber, 0, Buff + 1);
    AT45DB_Transfer(Buff, 1, Buff + 1, 3, AT45DB041_WRITE);
    AT45DB_WaitForFlash();
}

/*!
 * @brief Function that performs Flash formatting
 */
void AT45DB_FormatFlash(void)
{
	AT45DB_SectorZeroErase(FLASH_SECTOR_ZERO_A_CODE);
    AT45DB_SectorZeroErase(FLASH_SECTOR_ZERO_B_CODE);

    for(uint8_t i = SECTOR_START; i <= SECTOR_END; i++)
    {
    	AT45DB_SectorErase(i);
    }
}

/*!
 * @brief Function that loads a specific page to the internal buffer of the Flash memory chip
 *
 * @param PageNumber		Number of the page
 */
void AT45DB_LoadPageToInternalBuffer(uint16_t PageNumber)
{
    uint8_t Buff[4];

#ifdef MEMORY_BOUNDARY_CHECKS
    // Error check the parameter pageNumber
    if(PageNumber >= PAGE_COUNT) // Ex: 1M -> PAGE_COUNT = 512.. valid pageNumber 0-511
    {
    	AT45DB_MemoryBoundaryErrorCallback();
    }
#endif

    Buff[0] = FLASH_OPCODE_MAINP_TO_BUF;
    AT45DB_FillPageAddress(PageNumber, 0, Buff + 1);
    AT45DB_Transfer(Buff, 1, Buff + 1, 3, AT45DB041_WRITE);
    AT45DB_WaitForFlash();
}

/*!
 * @brief Function that writes to the Flash memory page. Data is entered relative to the offset
 *
 * @param PageNumber		Number of the page
 * @param Offset			Offset relative to the beginning of the page
 * @param Size				The size of the recorded information
 * @param pData				Pointer to an buffer of recorded information
 */
void AT45DB_WriteDataToFlash(uint16_t PageNumber, uint16_t Offset, uint16_t Size, void *pData)
{
    uint8_t Buff[4];

#ifdef MEMORY_BOUNDARY_CHECKS
    // Error check the parameter pageNumber
    if(PageNumber >= PAGE_COUNT) // Ex: 1M -> PAGE_COUNT = 512.. valid pageNumber 0-511
    {
    	AT45DB_MemoryBoundaryErrorCallback();
    }

    // Error check the parameters offset and dataSize
    if((Offset + Size - 1) >= BYTES_PER_PAGE) // Ex: 1M -> BYTES_PER_PAGE = 264 offset + dataSize MUST be less than 264 (0-263 valid)
    {
    	AT45DB_MemoryBoundaryErrorCallback();
    }
#endif

    AT45DB_LoadPageToInternalBuffer(PageNumber);
    Buff[0] = FLASH_OPCODE_MMP_PROG_TBUF;
    AT45DB_FillPageAddress(PageNumber, Offset, Buff + 1);
    AT45DB_Transfer(Buff, 4, pData, Size, AT45DB041_WRITE);
    AT45DB_WaitForFlash();
}

/*!
 * @brief Function that reads from a Flash memory page. Data is read relative to the offset
 *
 * @param PageNumber		Number of the page
 * @param Offset			Offset relative to the beginning of the page
 * @param Size				Size of the information to be read
 * @param pData				Pointer to the buffer where data will be read
 */
void AT45DB_ReadDataFromFlash(uint16_t PageNumber, uint16_t Offset, uint16_t Size, void *pData)
{
    uint8_t Buff[4];

#ifdef MEMORY_BOUNDARY_CHECKS
    // Error check the parameter pageNumber
    if(PageNumber >= PAGE_COUNT) // Ex: 1M -> PAGE_COUNT = 512.. valid pageNumber 0-511
    {
    	AT45DB_MemoryBoundaryErrorCallback();
    }
    // Error check the parameters offset and dataSize
    if((Offset + Size - 1) >= BYTES_PER_PAGE) // Ex: 1M -> BYTES_PER_PAGE = 264 offset + dataSize MUST be less than 264 (0-263 valid)
    {
    	AT45DB_MemoryBoundaryErrorCallback();
    }
#endif

    Buff[0] = FLASH_OPCODE_LOWF_READ;
    AT45DB_FillPageAddress(PageNumber, Offset, Buff + 1);
    AT45DB_Transfer(Buff, 4, pData, Size, AT45DB041_READ);
}

/*!
 * @brief Function that performs continuous data reading
 *
 * @param pData				Pointer to the buffer where data will be read
 * @param Address			Start address
 * @param Size				Size of the information to be read
 */
void AT45DB_FlashRawRead(uint8_t *pData, uint16_t Address, uint16_t Size)
{
    uint16_t page_number = (Address/BYTES_PER_PAGE);
    uint8_t high_byte = page_number >> (16 - READ_OFFSET_SHT_AMT);
    Address = (page_number << READ_OFFSET_SHT_AMT) | (Address % BYTES_PER_PAGE);
    uint8_t Buff[4] = {FLASH_OPCODE_LOWF_READ, high_byte, (uint8_t)(Address >> 8), (uint8_t)Address};
    AT45DB_Transfer(Buff, 4, pData, Size, AT45DB041_READ);
}

/*!
 * @brief Function that writes data to the internal buffer of the Flash memory chip
 *
 * @param pData				Pointer to the data buffer to be placed in the internal buffer
 * @param Offset			Offset relative to the beginning of the page
 * @param Size				The size of the recorded information
 */
void AT45DB_FlashWriteBuffer(uint8_t* pData, uint16_t Offset, uint16_t Size)
{
    uint8_t Buff[4];

    Buff[0] = FLASH_OPCODE_BUF_WRITE;
    AT45DB_FillPageAddress(0, Offset, Buff + 1);
    AT45DB_Transfer(Buff, 4, pData, Size, AT45DB041_WRITE);
    AT45DB_WaitForFlash();
}

/*!
 * @brief Function that writes the contents of the internal flash memory chip buffer to a Flash memory page
 *
 * @param Page
 */
void AT45DB_WriteBufferToPage(uint16_t Page)
{
    uint8_t Buff[4];

    Buff[0] = FLASH_OPCODE_BUF_TO_PAGE;
    AT45DB_FillPageAddress(Page, 0, Buff + 1);
    AT45DB_Transfer(Buff, 1, Buff + 1, 3, AT45DB041_WRITE);
    AT45DB_WaitForFlash();
}
