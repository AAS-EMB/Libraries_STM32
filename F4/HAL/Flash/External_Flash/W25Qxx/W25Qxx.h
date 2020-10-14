/*!
 * @file		W25Qxx.h
 *
 * @brief 		W25Qxx module implementation
 *
 * @author		Anosov Anton
 */

#ifndef EXTERNAL_FLASH_W25QXX_W25QXX_H_
#define EXTERNAL_FLASH_W25QXX_W25QXX_H_
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "W25Qxx_Conf.h"

#define W25Qxx_DUMMY_BYTE				0xA5

/* INSTRUCTIONS */
#define W25Qxx_WRITE_ENABLE				0x06
#define W25Qxx_VOLATILE_WRITE_ENABLE			0x50
#define W25Qxx_WRITE_DISABLE				0x04
#define W25Qxx_RELEASE_POWERDOWN			0xAB
#define W25Qxx_MAN_AND_DEV_ID				0x90
#define W25Qxx_JEDEC_ID					0x9F
#define W25Qxx_READ_UNIQUE_ID				0x4B
#define W25Qxx_READ_DATA				0x03
#define W25Qxx_FAST_READ_DATA				0x0B
#define W25Qxx_PAGE_PROGRAM				0x02
#define W25Qxx_SECTOR_ERASE				0x20
#define W25Qxx_32BLOCK_ERASE				0xD8
#define W25Qxx_64BLOCK_ERASE				0xD8
#define W25Qxx_CHIP_ERASE				0xC7
#define W25Qxx_READ_STATUS_REG_1			0x05
#define W25Qxx_WRITE_STATUS_REG_1			0x01
#define W25Qxx_READ_STATUS_REG_2			0x35
#define W25Qxx_WRITE_STATUS_REG_2			0x31
#define W25Qxx_READ_STATUS_REG_3			0x15
#define W25Qxx_WRITE_STATUS_REG_3			0x11
#define W25Qxx_READ_SFDP_REG				0x5A
#define W25Qxx_ERASE_SECURITY_REG			0x44
#define W25Qxx_PROGRAM_SECURITY_REG			0x42
#define W25Qxx_READ_SECURITY_REG			0x48
#define W25Qxx_GLOBAL_BLOCK_LOCK			0x7E
#define W25Qxx_GLOBAL_BLOCK_UNLOCK			0x98
#define W25Qxx_READ_BLOCK_LOCK				0x3D
#define W25Qxx_INDIVIDUAL_BLOCK_LOCK			0x36
#define W25Qxx_INDIVIDUAL_BLOCK_UNLOCK			0x39
#define W25Qxx_ERASE_PROGRAM_SUSPEND			0x75
#define W25Qxx_ERASE_PROGRAM_RESUME			0x7A
#define W25Qxx_POWERDOWN				0xB9
#define W25Qxx_ENABLE_RESET				0x66
#define W25Qxx_RESET_DEVICE				0x99

/* BITS MASK */
#define W25Qxx_MASK_BUSY				0x01
#define W25Qxx_MASK_WEL 				0x02
#define W25Qxx_MASK_BP0					0x04
#define W25Qxx_MASK_BP1					0x08
#define W25Qxx_MASK_BP2					0x10
#define W25Qxx_MASK_TB					0x20
#define W25Qxx_MASK_SEC					0x40
#define W25Qxx_MASK_SRP					0x80

#if defined(W25Q10)
	#define W25Qxx_BLOCK_COUNT			2
	#define W25Qxx_PAGE_SIZE			256
	#define W25Qxx_SECTOR_SIZE			4096
	#define W25Qxx_SECTOR_COUNT			(W25Qxx_BLOCK_COUNT*16)
	#define W25Qxx_PAGE_COUNT			((W25Qxx_SECTOR_COUNT*W25Qxx_SECTOR_SIZE)/W25Qxx_PAGE_SIZE)
	#define W25Qxx_BLOCK_SIZE			(W25Qxx_SECTOR_SIZE*16)
#elif defined(W25Q20)
	#define W25Qxx_BLOCK_COUNT			4
	#define W25Qxx_PAGE_SIZE			256
	#define W25Qxx_SECTOR_SIZE			4096
	#define W25Qxx_SECTOR_COUNT			(W25Qxx_BLOCK_COUNT*16)
	#define W25Qxx_PAGE_COUNT			((W25Qxx_SECTOR_COUNT*W25Qxx_SECTOR_SIZE)/W25Qxx_PAGE_SIZE)
	#define W25Qxx_BLOCK_SIZE			(W25Qxx_SECTOR_SIZE*16)
#elif defined(W25Q40)
	#define W25Qxx_BLOCK_COUNT			8
	#define W25Qxx_PAGE_SIZE			256
	#define W25Qxx_SECTOR_SIZE			4096
	#define W25Qxx_SECTOR_COUNT			(W25Qxx_BLOCK_COUNT*16)
	#define W25Qxx_PAGE_COUNT			((W25Qxx_SECTOR_COUNT*W25Qxx_SECTOR_SIZE)/W25Qxx_PAGE_SIZE)
	#define W25Qxx_BLOCK_SIZE			(W25Qxx_SECTOR_SIZE*16)
#elif defined(W25Q80)
	#define W25Qxx_BLOCK_COUNT			16
	#define W25Qxx_PAGE_SIZE			256
	#define W25Qxx_SECTOR_SIZE			4096
	#define W25Qxx_SECTOR_COUNT			(W25Qxx_BLOCK_COUNT*16)
	#define W25Qxx_PAGE_COUNT			((W25Qxx_SECTOR_COUNT*W25Qxx_SECTOR_SIZE)/W25Qxx_PAGE_SIZE)
	#define W25Qxx_BLOCK_SIZE			(W25Qxx_SECTOR_SIZE*16)
#elif defined(W25Q16)
	#define W25Qxx_BLOCK_COUNT			32
	#define W25Qxx_PAGE_SIZE			256
	#define W25Qxx_SECTOR_SIZE			4096
	#define W25Qxx_SECTOR_COUNT			(W25Qxx_BLOCK_COUNT*16)
	#define W25Qxx_PAGE_COUNT			((W25Qxx_SECTOR_COUNT*W25Qxx_SECTOR_SIZE)/W25Qxx_PAGE_SIZE)
	#define W25Qxx_BLOCK_SIZE			(W25Qxx_SECTOR_SIZE*16)
#elif defined(W25Q32)
	#define W25Qxx_BLOCK_COUNT			64
	#define W25Qxx_PAGE_SIZE			256
	#define W25Qxx_SECTOR_SIZE			4096
	#define W25Qxx_SECTOR_COUNT			(W25Qxx_BLOCK_COUNT*16)
	#define W25Qxx_PAGE_COUNT			((W25Qxx_SECTOR_COUNT*W25Qxx_SECTOR_SIZE)/W25Qxx_PAGE_SIZE)
	#define W25Qxx_BLOCK_SIZE			(W25Qxx_SECTOR_SIZE*16)
#elif defined(W25Q64)
	#define W25Qxx_BLOCK_COUNT			128
	#define W25Qxx_PAGE_SIZE			256
	#define W25Qxx_SECTOR_SIZE			4096
	#define W25Qxx_SECTOR_COUNT			(W25Qxx_BLOCK_COUNT*16)
	#define W25Qxx_PAGE_COUNT			((W25Qxx_SECTOR_COUNT*W25Qxx_SECTOR_SIZE)/W25Qxx_PAGE_SIZE)
	#define W25Qxx_BLOCK_SIZE			(W25Qxx_SECTOR_SIZE*16)
#elif defined(W25Q128)
	#define W25Qxx_BLOCK_COUNT			256
	#define W25Qxx_PAGE_SIZE			256
	#define W25Qxx_SECTOR_SIZE			4096
	#define W25Qxx_SECTOR_COUNT			(W25Qxx_BLOCK_COUNT*16)
	#define W25Qxx_PAGE_COUNT			((W25Qxx_SECTOR_COUNT*W25Qxx_SECTOR_SIZE)/W25Qxx_PAGE_SIZE)
	#define W25Qxx_BLOCK_SIZE			(W25Qxx_SECTOR_SIZE*16)
#elif defined(W25Q256)
	#define W25Qxx_BLOCK_COUNT			512
	#define W25Qxx_PAGE_SIZE			256
	#define W25Qxx_SECTOR_SIZE			4096
	#define W25Qxx_SECTOR_COUNT			(W25Qxx_BLOCK_COUNT*16)
	#define W25Qxx_PAGE_COUNT			((W25Qxx_SECTOR_COUNT*W25Qxx_SECTOR_SIZE)/W25Qxx_PAGE_SIZE)
	#define W25Qxx_BLOCK_SIZE			(W25Qxx_SECTOR_SIZE*16)
#elif defined(W25Q512)
	#define W25Qxx_BLOCK_COUNT			1024
	#define W25Qxx_PAGE_SIZE			256
	#define W25Qxx_SECTOR_SIZE			4096
	#define W25Qxx_SECTOR_COUNT			(W25Qxx_BLOCK_COUNT*16)
	#define W25Qxx_PAGE_COUNT			((W25Qxx_SECTOR_COUNT*W25Qxx_SECTOR_SIZE)/W25Qxx_PAGE_SIZE)
	#define W25Qxx_BLOCK_SIZE			(W25Qxx_SECTOR_SIZE*16)
#endif

typedef enum W25Qxx_StatusReg_e
{
	W25Qxx_STATUS_REG_1 = 1,
	W25Qxx_STATUS_REG_2,
	W25Qxx_STATUS_REG_3
}W25Qxx_StatusReg_t;

/*!
 * @brief The Read Status Register instructions allow the 8-bit Status Registers to be read
 *
 * @param W25Qxx_StatusReg		Number of register
 * @param W25Qxx_pGetStatus		Get status from register
 */
void W25Qxx_ReadStatusReg(W25Qxx_StatusReg_t W25Qxx_StatusReg, uint8_t *W25Qxx_pGetStatus);

/*!
 * @brief The Write Status Register instruction allows the 8-bit Status Registers to be written
 *
 * @param W25Qxx_StatusReg		Number of register
 * @param W25Qxx_SetStatus		Set status to register
 */
void W25Qxx_WriteStatusReg(W25Qxx_StatusReg_t W25Qxx_StatusReg, uint8_t W25Qxx_SetStatus);

/*!
 * @brief The Read Data instruction allows one or more data bytes to be sequentially read from the memory
 *
 * @param W25Qxx_PageNumber		Page number
 * @param W25Qxx_OffsetInByte	Offset
 * @param W25Qxx_Data			Pointer to the read data
 * @param W25Qxx_Size			Data read size
 */
void W25Qxx_ReadData(uint32_t W25Qxx_PageNumber, uint32_t W25Qxx_OffsetInByte, void *W25Qxx_pData, uint16_t W25Qxx_Size);

/*!
 * @brief The Fast Read instruction is similar to the Read Data instruction except that it can operate at the highest
 *        possible frequency of FR (see AC Electrical Characteristics)
 *
 * @param W25Qxx_PageNumber		Page number
 * @param W25Qxx_OffsetInByte	Offset
 * @param W25Qxx_Data			Pointer to the read data
 * @param W25Qxx_Size			Data read size
 */
void W25Qxx_FastReadData(uint32_t W25Qxx_PageNumber, uint32_t W25Qxx_OffsetInByte, void *W25Qxx_pData, uint16_t W25Qxx_Size);

/*!
 * @brief The Page Program instruction allows from one byte to 256 bytes (a page) of data to be programmed at previously erased (FFh) memory locations
 *
 * @param W25Qxx_PageNumber		Number page
 * @param W25Qxx_OffsetInByte	Offset
 * @param W25Qxx_pData			Pointer to the write data
 * @param W25Qxx_Size			Size data write
 */
void W25Qxx_PageProgram(uint32_t W25Qxx_PageNumber, uint32_t W25Qxx_OffsetInByte, void *W25Qxx_pData, uint16_t W25Qxx_Size);

/*!
 * @brief The Sector Erase instruction sets all memory within a specified sector (4K-bytes) to the erased state of all 1s (FFh)
 *
 * @param W25Qxx_SectorNumber	Sector number
 */
void W25Qxx_SectorErase(uint16_t W25Qxx_SectorNumber);

/*!
 * @brief The Block Erase instruction sets all memory within a specified block to the erased state of all 1s (FFh)
 *
 * @param W25Qxx_BlockNumber	Block number
 */
void W25Qxx_BlockErase(uint16_t W25Qxx_BlockNumber);

/*!
 * @brief The Chip Erase instruction sets all memory within the device to the erased state of all 1s (FFh)
 */
void W25Qxx_ChipErase(void);

/*!
 * @brief The Erase/Program Suspend instruction �75h�, allows the system to interrupt a Sector or Block Erase
 * 		  operation or a Page Program operation and then read from or program/erase data to, any other sectors or blocks
 */
void W25Qxx_EraseProgramSuspend(void);

/*!
 * @brief The Erase/Program Resume instruction �7Ah� must be written to resume the Sector or Block Erase
 * 		  operation or the Page Program operation after an Erase/Program Suspend
 */
void W25Qxx_EraseProgramResume(void);

/*!
 * @brief Although the standby current during normal operation is relatively low, standby current can be further
 * 		  reduced with the Power-down instruction
 */
void W25Qxx_PowerDown(void);

/*!
 * @brief The Release from Power-down instruction is a multi-purpose instruction
 */
void W25Qxx_ReleasePowerDown(void);

/*!
 * @brief The Read Manufacturer/Device ID instruction is an alternative to the Release from Power-down / Device ID
 * 		  instruction that provides both the JEDEC assigned manufacturer ID and the specific device ID
 *
 * @param W25Qxx_pManID			Pointer to the Manufacture ID
 * @param W25Qxx_pDeviceID		Pointer to the Device ID
 */
void W25Qxx_ReadManAndDevID(uint8_t *W25Qxx_pManID, uint8_t *W25Qxx_pDevID);

/*!
 * @brief The Read Unique ID Number instruction accesses a factory-set read-only 64-bit number that is unique to each device
 *
 * @param W25Qxx_pUiniqueID		Pointer to the Unique ID
 */
void W25Qxx_ReadUniqueId(uint8_t *W25Qxx_pUiniqueID);

/*!
 * @brief For  compatibility  reasons,  provides  several  instructions  to  electronically  determine  the identity  of  the  device
 *
 * @param W25Qxx_pManID			Pointer to the Manufacture ID
 * @param W25Qxx_pMemTypeID		Pointer to the Memory type ID
 * @param W25Qxx_pCapacityID	Pointer to the Capacity ID
 */
void W25Qxx_ReadJedecId(uint8_t *W25Qxx_pManID, uint8_t *W25Qxx_pMemTypeID, uint8_t *W25Qxx_pCapacityID);

#ifdef __cplusplus
}
#endif
#endif /* EXTERNAL_FLASH_W25QXX_W25QXX_H_ */
