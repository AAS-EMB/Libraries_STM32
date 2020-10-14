/*!
 * @file		W25Qxx.c
 *
 * @brief 		W25Qxx module implementation
 *
 * @author		Anosov Anton
 */

#include "W25Qxx.h"
#include "DebugPrint.h"

#define MAX_DELAY_TIME			100

/*!
 * @brief SPI transfer
 *
 * @param Data				Send byte
 *
 * @return				Receive byte
 */
static uint8_t W25Qxx_TransmitReceive(uint8_t Data)
{
	// IMPLEMENTATION
	uint8_t RecvByte;
	HAL_SPI_TransmitReceive(&W25Qxx_SPI, &Data, &RecvByte, sizeof(uint8_t), MAX_DELAY_TIME);
	return RecvByte;
}

/*!
 * @brief Waiting last operation
 */
static void W25Qxx_WaitingLastOperation(void)
{
	uint8_t Status = 0;

	W25Qxx_CHIP_ENABLE();
	W25Qxx_TransmitReceive(W25Qxx_READ_STATUS_REG_1);
	do
	{
		Status = W25Qxx_TransmitReceive(W25Qxx_DUMMY_BYTE);
	} while((Status & W25Qxx_MASK_BUSY) == W25Qxx_MASK_BUSY);
	W25Qxx_CHIP_DISABLE();
}

/*!
 * @brief Instruction sets the Write Enable Latch (WEL) bit in the Status Register to a 1
 */
static void W25Qxx_WriteEnable(void)
{
	/* Working with Flash */
	W25Qxx_BEGIN_CRITICAL_SECTION();
	W25Qxx_CHIP_ENABLE();
	W25Qxx_TransmitReceive(W25Qxx_WRITE_ENABLE);
	W25Qxx_CHIP_DISABLE();
	W25Qxx_END_CRITICAL_SECTION();
	W25Qxx_DELAY(1);
}

/*!
 * @brief The non-volatile Status Register bits described can also be written to as volatile bits.
 * 		  Write Enable for Volatile Status Register instruction will not set the Write Enable Latch (WEL) bit,
 * 		  it is only valid for the Write Status Register instruction to change the volatile Status Register bit values
 */
static void W25Qxx_VolatileWriteEnable(void)
{
	/* Working with Flash */
	W25Qxx_BEGIN_CRITICAL_SECTION();
	W25Qxx_CHIP_ENABLE();
	W25Qxx_TransmitReceive(W25Qxx_VOLATILE_WRITE_ENABLE);
	W25Qxx_CHIP_DISABLE();
	W25Qxx_END_CRITICAL_SECTION();
	W25Qxx_DELAY(1);
}

/*!
 * @brief Instruction sets the Write Enable Latch (WEL) bit in the Status Register to a 0
 */
static void W25Qxx_WriteDisable(void)
{
	/* Working with Flash */
	W25Qxx_BEGIN_CRITICAL_SECTION();
	W25Qxx_CHIP_ENABLE();
	W25Qxx_TransmitReceive(W25Qxx_WRITE_DISABLE);
	W25Qxx_CHIP_DISABLE();
	W25Qxx_END_CRITICAL_SECTION();
	W25Qxx_DELAY(1);
}

/*!
 * @brief The Read Status Register instructions allow the 8-bit Status Registers to be read
 *
 * @param W25Qxx_StatusReg		Number of register
 * @param W25Qxx_pGetStatus		Get status from register
 */
void W25Qxx_ReadStatusReg(W25Qxx_StatusReg_t W25Qxx_StatusReg, uint8_t *W25Qxx_pGetStatus)
{
	/* Check the parameters */
	assert_param(W25Qxx_pGetStatus);

	/* Working with Flash */
	W25Qxx_BEGIN_CRITICAL_SECTION();
	W25Qxx_CHIP_ENABLE();
	switch(W25Qxx_StatusReg)
	{
		case W25Qxx_STATUS_REG_1:
			*W25Qxx_pGetStatus = W25Qxx_TransmitReceive(W25Qxx_READ_STATUS_REG_1);
			break;
		case W25Qxx_STATUS_REG_2:
			*W25Qxx_pGetStatus = W25Qxx_TransmitReceive(W25Qxx_READ_STATUS_REG_2);
			break;
		case W25Qxx_STATUS_REG_3:
			*W25Qxx_pGetStatus = W25Qxx_TransmitReceive(W25Qxx_READ_STATUS_REG_3);
			break;
	}
	W25Qxx_CHIP_DISABLE();
	W25Qxx_END_CRITICAL_SECTION();
}

/*!
 * @brief The Write Status Register instruction allows the 8-bit Status Registers to be written
 *
 * @param W25Qxx_StatusReg		Number of register
 * @param W25Qxx_SetStatus		Set status to register
 */
void W25Qxx_WriteStatusReg(W25Qxx_StatusReg_t W25Qxx_StatusReg, uint8_t W25Qxx_SetStatus)
{
	/* Working with Flash */
	W25Qxx_BEGIN_CRITICAL_SECTION();
	W25Qxx_CHIP_ENABLE();
	switch(W25Qxx_StatusReg)
	{
		case W25Qxx_STATUS_REG_1:
			W25Qxx_TransmitReceive(W25Qxx_WRITE_STATUS_REG_1);
			break;
		case W25Qxx_STATUS_REG_2:
			W25Qxx_TransmitReceive(W25Qxx_WRITE_STATUS_REG_2);
			break;
		case W25Qxx_STATUS_REG_3:
			W25Qxx_TransmitReceive(W25Qxx_WRITE_STATUS_REG_3);
			break;
	}
	W25Qxx_TransmitReceive(W25Qxx_SetStatus);
	W25Qxx_CHIP_DISABLE();
	W25Qxx_END_CRITICAL_SECTION();
}

/*!
 * @brief The Read Data instruction allows one or more data bytes to be sequentially read from the memory
 *
 * @param W25Qxx_PageNumber		Page number
 * @param W25Qxx_OffsetInByte		Offset
 * @param W25Qxx_Data			Pointer to the read data
 * @param W25Qxx_Size			Data read size
 */
void W25Qxx_ReadData(uint32_t W25Qxx_PageNumber, uint32_t W25Qxx_OffsetInByte, void *W25Qxx_pData, uint16_t W25Qxx_Size)
{
	uint32_t PageAddress = W25Qxx_PageNumber*W25Qxx_PAGE_SIZE + W25Qxx_OffsetInByte;

	/* Check the parameters */
	assert_param(W25Qxx_pData);

	W25Qxx_WaitingLastOperation();

	/* Working with Flash */
	W25Qxx_BEGIN_CRITICAL_SECTION();
	W25Qxx_CHIP_ENABLE();
	W25Qxx_TransmitReceive(W25Qxx_READ_DATA);
#if (defined(W25Q256) || defined(W25Q512))
	W25Qxx_TransmitReceive((PageAddress & 0xFF000000) >> 24);
	W25Qxx_TransmitReceive((PageAddress & 0xFF0000) >> 16);
	W25Qxx_TransmitReceive((PageAddress & 0xFF00) >> 8);
	W25Qxx_TransmitReceive(PageAddress & 0xFF);
#else
	W25Qxx_TransmitReceive((PageAddress & 0xFF0000) >> 16);
	W25Qxx_TransmitReceive((PageAddress & 0xFF00) >> 8);
	W25Qxx_TransmitReceive(PageAddress & 0xFF);
#endif
	HAL_SPI_Receive(&W25Qxx_SPI, (uint8_t*)W25Qxx_pData, W25Qxx_Size, MAX_DELAY_TIME);
	W25Qxx_CHIP_DISABLE();
	W25Qxx_END_CRITICAL_SECTION();
	W25Qxx_DELAY(10);
}

/*!
 * @brief The Fast Read instruction is similar to the Read Data instruction except that it can operate at the highest
 *        possible frequency of FR (see AC Electrical Characteristics)
 *
 * @param W25Qxx_PageNumber		Page number
 * @param W25Qxx_OffsetInByte		Offset
 * @param W25Qxx_Data			Pointer to the read data
 * @param W25Qxx_Size			Data read size
 */
void W25Qxx_FastReadData(uint32_t W25Qxx_PageNumber, uint32_t W25Qxx_OffsetInByte, void *W25Qxx_pData, uint16_t W25Qxx_Size)
{
	uint32_t PageAddress = W25Qxx_PageNumber*W25Qxx_PAGE_SIZE + W25Qxx_OffsetInByte;

	/* Check the parameters */
	assert_param(W25Qxx_pData);

	W25Qxx_WaitingLastOperation();

	/* Working with Flash */
	W25Qxx_BEGIN_CRITICAL_SECTION();
	W25Qxx_CHIP_ENABLE();
	W25Qxx_TransmitReceive(W25Qxx_FAST_READ_DATA);
#if (defined(W25Q256) || defined(W25Q512))
	W25Qxx_TransmitReceive((PageAddress & 0xFF000000) >> 24);
	W25Qxx_TransmitReceive((PageAddress & 0xFF0000) >> 16);
	W25Qxx_TransmitReceive((PageAddress & 0xFF00) >> 8);
	W25Qxx_TransmitReceive(PageAddress & 0xFF);
#else
	W25Qxx_TransmitReceive((PageAddress & 0xFF0000) >> 16);
	W25Qxx_TransmitReceive((PageAddress & 0xFF00) >> 8);
	W25Qxx_TransmitReceive(PageAddress & 0xFF);
#endif
	W25Qxx_TransmitReceive(0);
	HAL_SPI_Receive(&W25Qxx_SPI, (uint8_t*)W25Qxx_pData, W25Qxx_Size, MAX_DELAY_TIME);
	W25Qxx_CHIP_DISABLE();
	W25Qxx_END_CRITICAL_SECTION();
	W25Qxx_DELAY(10);
}

/*!
 * @brief The Page Program instruction allows from one byte to 256 bytes (a page) of data to be programmed at previously erased (FFh) memory locations
 *
 * @param W25Qxx_PageNumber		Number page
 * @param W25Qxx_OffsetInByte		Offset
 * @param W25Qxx_pData			Pointer to the write data
 * @param W25Qxx_Size			Size data write
 */
void W25Qxx_PageProgram(uint32_t W25Qxx_PageNumber, uint32_t W25Qxx_OffsetInByte, void *W25Qxx_pData, uint16_t W25Qxx_Size)
{
	uint32_t PageAddress = W25Qxx_PageNumber*W25Qxx_PAGE_SIZE + W25Qxx_OffsetInByte;

	/* Check the parameters */
	assert_param(W25Qxx_pData);

	W25Qxx_WaitingLastOperation();

	/* Working with Flash */
	W25Qxx_BEGIN_CRITICAL_SECTION();
#if defined(DEBUGPRINT)
	DPRINTF("W25Qxx PageProgram %u begin...\r\n", W25Qxx_PageNumber);
#endif
	W25Qxx_WriteEnable();
	W25Qxx_CHIP_ENABLE();
	W25Qxx_TransmitReceive(W25Qxx_PAGE_PROGRAM);
#if (defined(W25Q256) || defined(W25Q512))
	W25Qxx_TransmitReceive((PageAddress & 0xFF000000) >> 24);
	W25Qxx_TransmitReceive((PageAddress & 0xFF0000) >> 16);
	W25Qxx_TransmitReceive((PageAddress & 0xFF00) >> 8);
	W25Qxx_TransmitReceive(PageAddress & 0xFF);
#else
	W25Qxx_TransmitReceive((PageAddress & 0xFF0000) >> 16);
	W25Qxx_TransmitReceive((PageAddress & 0xFF00) >> 8);
	W25Qxx_TransmitReceive(PageAddress & 0xFF);
#endif
	HAL_SPI_Transmit(&W25Qxx_SPI, (uint8_t*)W25Qxx_pData, W25Qxx_Size, MAX_DELAY_TIME);
	W25Qxx_CHIP_DISABLE();
#if defined(DEBUGPRINT)
	DPRINTF("W25Qxx PageProgram %u complete...\r\n", W25Qxx_PageNumber);
#endif
	W25Qxx_WriteDisable();
	W25Qxx_END_CRITICAL_SECTION();
	W25Qxx_DELAY(10);
}

/*!
 * @brief The Sector Erase instruction sets all memory within a specified sector (4K-bytes) to the erased state of all 1s (FFh)
 *
 * @param W25Qxx_SectorNumber		Sector number
 */
void W25Qxx_SectorErase(uint16_t W25Qxx_SectorNumber)
{
	uint32_t SectorAddress = W25Qxx_SectorNumber*W25Qxx_SECTOR_SIZE;

	W25Qxx_WaitingLastOperation();

	/* Working with Flash */
	W25Qxx_BEGIN_CRITICAL_SECTION();
#if defined(DEBUGPRINT)
	DPRINTF("W25Qxx SectorErase %u begin...\r\n", W25Qxx_SectorNumber);
#endif
	W25Qxx_WriteEnable();
	W25Qxx_CHIP_ENABLE();
	W25Qxx_TransmitReceive(W25Qxx_SECTOR_ERASE);
#if (defined(W25Q256) || defined(W25Q512))
	W25Qxx_TransmitReceive((SectorAddress & 0xFF000000) >> 24);
	W25Qxx_TransmitReceive((SectorAddress & 0xFF0000) >> 16);
	W25Qxx_TransmitReceive((SectorAddress & 0xFF00) >> 8);
	W25Qxx_TransmitReceive(SectorAddress & 0xFF);
#else
	W25Qxx_TransmitReceive((SectorAddress & 0xFF0000) >> 16);
	W25Qxx_TransmitReceive((SectorAddress & 0xFF00) >> 8);
	W25Qxx_TransmitReceive(SectorAddress & 0xFF);
#endif
	W25Qxx_CHIP_DISABLE();
#if defined(DEBUGPRINT)
	DPRINTF("W25Qxx SectorErase %u complete...\r\n", W25Qxx_SectorNumber);
#endif
	W25Qxx_WriteDisable();
	W25Qxx_END_CRITICAL_SECTION();
	W25Qxx_DELAY(10);
}

/*!
 * @brief The Block Erase instruction sets all memory within a specified block to the erased state of all 1s (FFh)
 *
 * @param W25Qxx_BlockNumber		Block number
 */
void W25Qxx_BlockErase(uint16_t W25Qxx_BlockNumber)
{
	uint32_t BlockAddress = W25Qxx_BlockNumber*W25Qxx_BLOCK_SIZE;

	W25Qxx_WaitingLastOperation();

	/* Working with Flash */
	W25Qxx_BEGIN_CRITICAL_SECTION();
#if defined(DEBUGPRINT)
	DPRINTF("W25Qxx BlockErase %u begin...\r\n", W25Qxx_BlockNumber);
#endif
	W25Qxx_WriteEnable();
	W25Qxx_CHIP_ENABLE();
	W25Qxx_TransmitReceive(W25Qxx_32BLOCK_ERASE);
#if (defined(W25Q256) || defined(W25Q512))
	W25Qxx_TransmitReceive((BlockAddress & 0xFF000000) >> 24);
	W25Qxx_TransmitReceive((BlockAddress & 0xFF0000) >> 16);
	W25Qxx_TransmitReceive((BlockAddress & 0xFF00) >> 8);
	W25Qxx_TransmitReceive(BlockAddress & 0xFF);
#else
	W25Qxx_TransmitReceive((BlockAddress & 0xFF0000) >> 16);
	W25Qxx_TransmitReceive((BlockAddress & 0xFF00) >> 8);
	W25Qxx_TransmitReceive(BlockAddress & 0xFF);
#endif
	W25Qxx_CHIP_DISABLE();
#if defined(DEBUGPRINT)
	DPRINTF("W25Qxx BlockErase %u complete...\r\n", W25Qxx_BlockNumber);
#endif
	W25Qxx_WriteDisable();
	W25Qxx_END_CRITICAL_SECTION();
	W25Qxx_DELAY(10);
}

/*!
 * @brief The Chip Erase instruction sets all memory within the device to the erased state of all 1s (FFh)
 */
void W25Qxx_ChipErase(void)
{
	W25Qxx_WaitingLastOperation();

	/* Working with Flash */
	W25Qxx_BEGIN_CRITICAL_SECTION();
#if defined(DEBUGPRINT)
	DPRINT("W25Qxx ChipErase begin!\r\n");
#endif
	W25Qxx_WriteEnable();
	W25Qxx_CHIP_ENABLE();
	W25Qxx_TransmitReceive(W25Qxx_CHIP_ERASE);
	W25Qxx_CHIP_DISABLE();
#if defined(DEBUGPRINT)
	DPRINT("W25Qxx ChipErase complete!\r\n");
#endif
	W25Qxx_WriteDisable();
	W25Qxx_END_CRITICAL_SECTION();
	W25Qxx_DELAY(3000);
}

/*!
 * @brief The Erase/Program Suspend instruction �75h�, allows the system to interrupt a Sector or Block Erase
 * 		  operation or a Page Program operation and then read from or program/erase data to, any other sectors or blocks
 */
void W25Qxx_EraseProgramSuspend(void)
{
	/* Working with Flash */
	W25Qxx_BEGIN_CRITICAL_SECTION();
	W25Qxx_CHIP_ENABLE();
	W25Qxx_TransmitReceive(W25Qxx_ERASE_PROGRAM_SUSPEND);
	W25Qxx_CHIP_DISABLE();
	W25Qxx_END_CRITICAL_SECTION();
	W25Qxx_DELAY(10);
}

/*!
 * @brief The Erase/Program Resume instruction �7Ah� must be written to resume the Sector or Block Erase
 * 		  operation or the Page Program operation after an Erase/Program Suspend
 */
void W25Qxx_EraseProgramResume(void)
{
	/* Working with Flash */
	W25Qxx_BEGIN_CRITICAL_SECTION();
	W25Qxx_CHIP_ENABLE();
	W25Qxx_TransmitReceive(W25Qxx_ERASE_PROGRAM_RESUME);
	W25Qxx_CHIP_DISABLE();
	W25Qxx_END_CRITICAL_SECTION();
	W25Qxx_DELAY(10);
}

/*!
 * @brief Although the standby current during normal operation is relatively low, standby current can be further
 * 		  reduced with the Power-down instruction
 */
void W25Qxx_PowerDown(void)
{
	/* Working with Flash */
	W25Qxx_BEGIN_CRITICAL_SECTION();
	W25Qxx_CHIP_ENABLE();
	W25Qxx_TransmitReceive(W25Qxx_POWERDOWN);
	W25Qxx_CHIP_DISABLE();
	W25Qxx_END_CRITICAL_SECTION();
	W25Qxx_DELAY(10);
}

/*!
 * @brief The Release from Power-down instruction is a multi-purpose instruction
 */
void W25Qxx_ReleasePowerDown(void)
{
	/* Working with Flash */
	W25Qxx_BEGIN_CRITICAL_SECTION();
	W25Qxx_CHIP_ENABLE();
	W25Qxx_TransmitReceive(W25Qxx_RELEASE_POWERDOWN);
	W25Qxx_CHIP_DISABLE();
	W25Qxx_END_CRITICAL_SECTION();
	W25Qxx_DELAY(10);
}

/*!
 * @brief The Read Manufacturer/Device ID instruction is an alternative to the Release from Power-down / Device ID
 * 		  instruction that provides both the JEDEC assigned manufacturer ID and the specific device ID
 *
 * @param W25Qxx_pManID			Pointer to the Manufacture ID
 * @param W25Qxx_pDeviceID		Pointer to the Device ID
 */
void W25Qxx_ReadManAndDevID(uint8_t *W25Qxx_pManID, uint8_t *W25Qxx_pDevID)
{
	/* Check the parameters */
	assert_param(W25Qxx_pManID);
	assert_param(W25Qxx_pDevID);

	/* Working with Flash */
	W25Qxx_BEGIN_CRITICAL_SECTION();
	W25Qxx_CHIP_ENABLE();
	W25Qxx_TransmitReceive(W25Qxx_MAN_AND_DEV_ID);
	W25Qxx_TransmitReceive(0);
	W25Qxx_TransmitReceive(0);
	W25Qxx_TransmitReceive(0);
	*W25Qxx_pManID = W25Qxx_TransmitReceive(W25Qxx_DUMMY_BYTE);
	*W25Qxx_pDevID = W25Qxx_TransmitReceive(W25Qxx_DUMMY_BYTE);
#if defined(DEBUGPRINT)
	DPRINT("W25Qxx Read Manufacture and Device ID complete\r\n");
	DPRINTF("W25Qxx Manufacture ID: %u\r\n", *W25Qxx_pManID);
	DPRINTF("W25Qxx Device ID: %u\r\n", *W25Qxx_pDevID);
#endif
	W25Qxx_CHIP_DISABLE();
	W25Qxx_END_CRITICAL_SECTION();
}

/*!
 * @brief The Read Unique ID Number instruction accesses a factory-set read-only 64-bit number that is unique to each device
 *
 * @param W25Qxx_pUiniqueID		Pointer to the Unique ID
 */
void W25Qxx_ReadUniqueId(uint8_t *W25Qxx_pUiniqueID)
{
	const uint8_t SendBytes = 12;

	/* Check the parameters */
	assert_param(W25Qxx_pUiniqueID);

	/* Working with Flash */
	W25Qxx_BEGIN_CRITICAL_SECTION();
	W25Qxx_CHIP_ENABLE();
	W25Qxx_TransmitReceive(W25Qxx_READ_UNIQUE_ID);
	for(uint8_t Count = 0; Count < SendBytes; Count++)
	{
		if(Count < 4) W25Qxx_TransmitReceive(W25Qxx_DUMMY_BYTE);
		else W25Qxx_pUiniqueID[Count - 4] = W25Qxx_TransmitReceive(W25Qxx_DUMMY_BYTE);
	}
#if defined(DEBUGPRINT)
	DPRINT("W25Qxx Read Unique ID complete\r\n");
	DPRINTF("W25Qxx Unique ID byte 1: %u\r\n", *W25Qxx_pUiniqueID);
	DPRINTF("W25Qxx Unique ID byte 2: %u\r\n", *(W25Qxx_pUiniqueID + 1));
	DPRINTF("W25Qxx Unique ID byte 3: %u\r\n", *(W25Qxx_pUiniqueID + 2));
	DPRINTF("W25Qxx Unique ID byte 4: %u\r\n", *(W25Qxx_pUiniqueID + 3));
	DPRINTF("W25Qxx Unique ID byte 5: %u\r\n", *(W25Qxx_pUiniqueID + 4));
	DPRINTF("W25Qxx Unique ID byte 6: %u\r\n", *(W25Qxx_pUiniqueID + 5));
	DPRINTF("W25Qxx Unique ID byte 7: %u\r\n", *(W25Qxx_pUiniqueID + 6));
	DPRINTF("W25Qxx Unique ID byte 8: %u\r\n", *(W25Qxx_pUiniqueID + 7));
#endif
	W25Qxx_CHIP_DISABLE();
	W25Qxx_END_CRITICAL_SECTION();
}

/*!
 * @brief For  compatibility  reasons,  provides  several  instructions  to  electronically  determine  the identity  of  the  device
 *
 * @param W25Qxx_pManID			Pointer to the Manufacture ID
 * @param W25Qxx_pMemTypeID		Pointer to the Memory type ID
 * @param W25Qxx_pCapacityID		Pointer to the Capacity ID
 */
void W25Qxx_ReadJedecId(uint8_t *W25Qxx_pManID, uint8_t *W25Qxx_pMemTypeID, uint8_t *W25Qxx_pCapacityID)
{
	/* Check the parameters */
	assert_param(W25Qxx_pManID);
	assert_param(W25Qxx_pMemTypeID);
	assert_param(W25Qxx_pCapacityID);

	/* Working with Flash */
	W25Qxx_BEGIN_CRITICAL_SECTION();
	W25Qxx_CHIP_ENABLE();
	W25Qxx_TransmitReceive(W25Qxx_JEDEC_ID);
	*W25Qxx_pManID = W25Qxx_TransmitReceive(W25Qxx_DUMMY_BYTE);
	*W25Qxx_pMemTypeID = W25Qxx_TransmitReceive(W25Qxx_DUMMY_BYTE);
	*W25Qxx_pCapacityID = W25Qxx_TransmitReceive(W25Qxx_DUMMY_BYTE);
#if defined(DEBUGPRINT)
	DPRINT("W25Qxx ReadJedecId complete\r\n");
	DPRINTF("W25Qxx Manufacture ID: %u\r\n", *W25Qxx_pManID);
	DPRINTF("W25Qxx Memory Type ID: %u\r\n", *W25Qxx_pMemTypeID);
	DPRINTF("W25Qxx Capacity ID: %u\r\n", *W25Qxx_pCapacityID);
#endif
	W25Qxx_CHIP_DISABLE();
	W25Qxx_END_CRITICAL_SECTION();
}
