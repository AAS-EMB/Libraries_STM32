/*!
 * @file      AT45DB.h
 *
 * @brief     External flash AT45DB implementation
 *
 * @author    Anosov Anton
 */

#ifndef FLASH_MEM_H_
#define FLASH_MEM_H_
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "spi.h"
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

/* Configuration SPI */
extern SPI_HandleTypeDef  hspi2;
#define AT45DB_CS_PORT	  GPIOA
#define AT45DB_CS_PIN	  GPIO_PIN_4
#define AT45DB_CS_LOW()   HAL_GPIO_WritePin(AT45DB_CS_PORT, AT45DB_CS_PIN, GPIO_PIN_RESET)
#define AT45DB_CS_HIGH()  HAL_GPIO_WritePin(AT45DB_CS_PORT, AT45DB_CS_PIN, GPIO_PIN_SET)
#define AT45DB_SPI 		  hspi2

typedef enum AT45DB_Operation_s
{
	AT45DB041_WRITE = 0,
	AT45DB041_READ
}AT45DB_Operation_t;

/* Выбор микросхемы флеш памяти */
#if defined(FLASH_CHIP_1M)      // Used to identify a 1M Flash Chip (AT45DB011D)
    #define FLASH_CHIP 1
    #define FLASH_CHIP_STR  "\x01"
#elif defined(FLASH_CHIP_2M)    // Used to identify a 2M Flash Chip (AT45DB021E)
    #define FLASH_CHIP 2
    #define FLASH_CHIP_STR  "\x02"
#elif defined(FLASH_CHIP_4M)    // Used to identify a 4M Flash Chip (AT45DB041E)
    #define FLASH_CHIP 4
    #define FLASH_CHIP_STR  "\x04"
#elif defined(FLASH_CHIP_8M)    // Used to identify a 8M Flash Chip (AT45DB081E)
    #define FLASH_CHIP 8
    #define FLASH_CHIP_STR  "\x08"
#elif defined(FLASH_CHIP_16M)   // Used to identify a 16M Flash Chip (AT45DB161E)
    #define FLASH_CHIP 16
    #define FLASH_CHIP_STR  "\x10"
#elif defined(FLASH_CHIP_32M)   // Used to identify a 32M Flash Chip (AT45DB321E)
    #define FLASH_CHIP 32
    #define FLASH_CHIP_STR  "\x20"
#endif

#if defined(FLASH_CHIP_1M)
    #define MAN_FAM_DEN_VAL 0x22       // Used for Chip Identity (see datasheet)
    #define PAGE_COUNT 512             // Number of pages in the chip
    #define BYTES_PER_PAGE 264         // Bytes per page of the chip
    #define MAP_BYTES 192              // Bytes required to make 'node' usage (map) -> (((PAGE_COUNT - PAGE_PER_SECTOR) * BYTES_PER_PAGE) / NODE_SIZE_PARENT) / 8bits
    #define MAP_PAGES 1                // Pages required to hold 'node' usage (map) -> CEILING(MAP_BYTES / BYTES_PER_PAGE)
    #define NODE_PARENT_PER_PAGE 4     // Number of parent nodes per page -> BYTES_PER_PAGE / NODE_SIZE_PARENT
	#define NODE_CHILD_MAX_NODE 2	   // Last valid child node block (due to size of child = size of parent * 2)
	#define NODE_PER_PAGE 2            // Number of nodes per page
    #define BLOCK_COUNT 64             // Number of blocks in the chip
    #define SECTOR_START 1             // The first whole sector number in the chip
    #define SECTOR_END 3               // The last whole sector number in the chip
    #define PAGE_PER_SECTOR 128        // Number of pages per sector in the chip

    #define SECTOR_ERASE_0_SHT_AMT 12  // The shift amount used for a sector zero part erase (see comments below)
    #define SECTOR_ERASE_N_SHT_AMT 16  // The shift amount used for a sector erase (see comments below)
    #define BLOCK_ERASE_SHT_AMT 12     // The shift amount used for a block erase (see comments below)
    #define PAGE_ERASE_SHT_AMT 9       // The shift amount used for a page erase (see comments below)
    #define WRITE_SHT_AMT 9            // The shift amount used for a write operation (see comments below)
    #define READ_OFFSET_SHT_AMT 9      // The shift amount used for a read operation (see comments below)

    // sector erase -> 264size -> OP: 0x7C -> 0a/0b -> 3 address bytes -> 5 D/C, 7 Page address bits (PA9-PA3), 12 D/C -> PA3=0 -> 0a, PA3=1 -> 0b
    // sector erase -> 264size -> OP: 0x7C -> 1/3 0> 3 address bytes -> 6 D/C, 2 Page address bits (PA8-PA7), 16 D/C
    // sector 0a -> 8 pages, sector 0b -> 120 pages, sector 1/3 -> 128 page
    // block erase -> 264size -> OP: 0x50 -> 3 address bytes -> 6 D/C, 6 Block Num, 12 D/C -> 64 Blocks
    // page erase -> 264size -> OP: 0x81 -> 3 address bytes -> 6 D/C, 9 Page Num, 9 D/C -> 512 Pages

    // Write_p1 -> 264size -> MMP to Buffer T -> OP: 0x53 -> 3 address bytes -> 6 D/C, 9 Page Address, 9 D/C
    // write_p2 -> 264size -> MMP PROG T Buffer -> OP: 0x82 -> 3 address bytes -> 6 D/C, 9 Page Address, 9 Buffer Address

    // Read -> 528size -> Low Freq Read -> 0P: 0x03 -> 3 address bytes -> 9 Page Address, 9 Offset, 6 D/C ?
#elif defined(FLASH_CHIP_2M)
    #define MAN_FAM_DEN_VAL 0x23       // Used for Chip Identity (see datasheet)
    #define PAGE_COUNT 1024            // Number of pages in the chip
    #define BYTES_PER_PAGE 264         // Bytes per page of the chip
    #define MAP_BYTES 448              // Bytes required to make 'node' usage (map) -> (((PAGE_COUNT - PAGE_PER_SECTOR) * BYTES_PER_PAGE) / NODE_SIZE_PARENT) / 8bits
    #define MAP_PAGES 2                // Pages required to hold 'node' usage (map) -> CEILING(MAP_BYTES / BYTES_PER_PAGE)
    #define NODE_PARENT_PER_PAGE 4     // Number of parent nodes per page -> BYTES_PER_PAGE / NODE_SIZE_PARENT
	#define NODE_CHILD_MAX_NODE 2	   // Last valid child node block (due to size of child = size of parent * 2)
    #define NODE_PER_PAGE 2            // Number of nodes per page
    #define BLOCK_COUNT 128            // Number of blocks in the chip
    #define SECTOR_START 1             // The first whole sector number in the chip
    #define SECTOR_END 7               // The last whole sector number in the chip
    #define PAGE_PER_SECTOR 128        // Number of pages per sector in the chip

    #define SECTOR_ERASE_0_SHT_AMT 12  // The shift amount used for a sector zero part erase (see comments below)
    #define SECTOR_ERASE_N_SHT_AMT 16  // The shift amount used for a sector erase (see comments below)
    #define BLOCK_ERASE_SHT_AMT 12     // The shift amount used for a block erase (see comments below)
    #define PAGE_ERASE_SHT_AMT 9       // The shift amount used for a page erase (see comments below)
    #define WRITE_SHT_AMT 9            // The shift amount used for a write operation (see comments below)
    #define READ_OFFSET_SHT_AMT 9      // The shift amount used for a read operation (see comments below)

    // sector erase -> 264size -> OP: 0x7C -> 0a/0b -> 3 address bytes -> 5 D/C, 7 Sector Address (PA9-PA3), 12 D/C -> PA3=0 -> 0a, PA3=1 -> 0b
    // sector erase -> 264size -> OP: 0x7C -> 1/7 -> 3 address bytes -> 5 D/C, 3 Sector Address Bits, 16 D/C
    // sector 0a -> 8 pages, sector 0b -> 120 pages, sector 1/7 -> 128 pages
    // block erase -> 264size -> OP: 0x50 -> 3 address bytes -> 5 D/C, 7 Block Num, 12 D/C -> 128 Blocks
    // page erase -> 264size -> OP: 0x81 -> 3 address bytes -> 5 D/C, 10 Page Num, 9 D/C -> 1024 pages

    // Write_p1 -> 264size -> MMP to Buffer T -> OP: 0x53 -> 3 address bytes -> 5 D/C, 10 Page Address, 9 D/C
    // write_p2 -> 264size -> MMP PROG T Buffer -> OP: 0x82 -> 3 address bytes -> 5 D/C, 10 Page Address, 9 Buffer Address

    // Read -> 528size -> Low Freq Read -> 0P: 0x03 -> 3 address bytes -> 10 Page Address, 9 Offset, 5 D/C ?
#elif defined(FLASH_CHIP_4M)
    #define MAN_FAM_DEN_VAL 0x24	   // Used for Chip Identity (see datasheet)
    #define PAGE_COUNT 2048			   // Number of pages in the chip
    #define BYTES_PER_PAGE 256		   // Bytes per page of the chip
    #define MAP_BYTES 896			   // Bytes required to make 'node' usage (map) -> (((PAGE_COUNT - PAGE_PER_SECTOR) * BYTES_PER_PAGE) / NODE_SIZE_PARENT) / 8bits
    #define MAP_PAGES 4				   // Pages required to hold 'node' usage (map) -> CEILING(MAP_BYTES / BYTES_PER_PAGE)
    #define NODE_PARENT_PER_PAGE 4     // Number of parent nodes per page -> BYTES_PER_PAGE / NODE_SIZE_PARENT
	#define NODE_CHILD_MAX_NODE 2	   // Last valid child node block (due to size of child = size of parent * 2)
	#define NODE_PER_PAGE 2            // Number of nodes per page
    #define BLOCK_COUNT 256            // Number of blocks in the chip
    #define SECTOR_START 1             // The first whole sector number in the chip
    #define SECTOR_END 7               // The last whole sector number in the chip
    #define PAGE_PER_SECTOR 256        // Number of pages per sector in the chip

    #define SECTOR_ERASE_0_SHT_AMT 12  // The shift amount used for a sector zero part erase (see comments below)
    #define SECTOR_ERASE_N_SHT_AMT 17  // The shift amount used for a sector erase (see comments below)
    #define BLOCK_ERASE_SHT_AMT 12     // The shift amount used for a block erase (see comments below)
    #define PAGE_ERASE_SHT_AMT 9       // The shift amount used for a page erase (see comments below)
    #define WRITE_SHT_AMT 9            // The shift amount used for a write operation (see comments below)
    #define READ_OFFSET_SHT_AMT 9      // The shift amount used for a read operation (see comments below)

    // sector erase -> 264size -> OP: 0x7C -> 0a/0b -> 3 address bytes -> 4 D/C, 8 Sector Address (PA10-PA3), 12 D/C -> PA3=0 -> 0a, PA3=1 -> 0b
    // sector erase -> 264size -> OP: 0x7C -> 1/7 -> 3 address bytes -> 4 D/C, 3 Sector Address Bits, 17 D/C
    // sector 0a -> 8 pages, sector 0b -> 248 pages, sector 1/7 -> 256 pages
    // block erase -> 264size -> OP: 0x50 -> 3 address bytes -> 4 D/C, 8 Block Num, 12 D/C -> 256 Blocks
    // page erase -> 264size -> OP: 0x81 -> 3 address bytes -> 4 D/C, 11 Page Num, 9 D/C -> 2048 Pages

    // Write_p1 -> 264size -> MMP to Buffer T -> OP: 0x53 -> 3 address bytes -> 4 D/C, 11 Page Address, 9 D/C
    // write_p2 -> 264size -> MMP PROG T Buffer -> OP: 0x82 -> 3 address bytes -> 4 D/C, 11 Page Address, 9 Buffer Address

    // Read -> 528size -> Low Freq Read -> 0P: 0x03 -> 3 address bytes -> 11 Page Address, 9 Offset, 4 D/C ?
#elif defined(FLASH_CHIP_8M)
    #define MAN_FAM_DEN_VAL 0x25       // Used for Chip Identity (see datasheet)
    #define PAGE_COUNT 4096            // Number of pages in the chip
    #define BYTES_PER_PAGE 264         // Bytes per page of the chip
    #define MAP_BYTES 1920             // Bytes required to make 'node' usage (map) -> (((PAGE_COUNT - PAGE_PER_SECTOR) * BYTES_PER_PAGE) / NODE_SIZE_PARENT) / 8bits
    #define MAP_PAGES 8                // Pages required to hold 'node' usage (map) -> CEILING(MAP_BYTES / BYTES_PER_PAGE)
    #define NODE_PARENT_PER_PAGE 4     // Number of parent nodes per page -> BYTES_PER_PAGE / NODE_SIZE_PARENT
	#define NODE_CHILD_MAX_NODE 2	   // Last valid child node block (due to size of child = size of parent * 2)
	#define NODE_PER_PAGE 2            // Number of nodes per page
    #define BLOCK_COUNT 512            // Number of blocks in the chip
    #define SECTOR_START 1             // The first whole sector number in the chip
    #define SECTOR_END 15               // The last whole sector number in the chip
    #define PAGE_PER_SECTOR 256        // Number of pages per sector in the chip

    #define SECTOR_ERASE_0_SHT_AMT 12  // The shift amount used for a sector zero part erase (see comments below)
    #define SECTOR_ERASE_N_SHT_AMT 17  // The shift amount used for a sector erase (see comments below)
    #define BLOCK_ERASE_SHT_AMT 12     // The shift amount used for a block erase (see comments below)
    #define PAGE_ERASE_SHT_AMT 9       // The shift amount used for a page erase (see comments below)
    #define WRITE_SHT_AMT 9            // The shift amount used for a write operation (see comments below)
    #define READ_OFFSET_SHT_AMT 9      // The shift amount used for a read operation (see comments below)

    // sector erase -> 264size -> OP: 0x7C -> 0a/0b -> 3 address bytes -> 3 D/C, 9 Sector Address (PA11-PA3), 12 D/C -> PA3=0 -> 0a, PA3=1 -> 0b
    // sector erase -> 264size -> OP: 0x7C -> 1/15 -> 3 address bytes -> 3 D/C, 4 Sector Address Bits, 17 D/C
    // sector 0a -> 8 pages, sector 0b -> 248 pages, sector 1/15 -> 256 pages
    // block erase -> 264size -> OP: 0x50 -> 3 address bytes -> 3 D/C, 9 Block Num, 12 D/C -> 512 Blocks
    // page erase -> 264size -> OP: 0x81 -> 3 address bytes -> 3 D/C, 12 Page Num, 9 D/C -> 4096 Pages

    // Write_p1 -> 264size -> MMP to Buffer T -> OP: 0x53 -> 3 address bytes -> 3 D/C, 12 Page Address, 9 D/C
    // write_p2 -> 264size -> MMP PROG T Buffer -> OP: 0x82 -> 3 address bytes -> 3 D/C, 12 Page Address, 9 Buffer Address

    // Read -> 528size -> Low Freq Read -> 0P: 0x03 -> 3 address bytes -> 12 Page Address, 9 Offset, 3 D/C ?
#elif defined(FLASH_CHIP_16M)
    #define MAN_FAM_DEN_VAL 0x26       // Used for Chip Identity (see datasheet)
    #define PAGE_COUNT 4096            // Number of pages in the chip
    #define BYTES_PER_PAGE 528         // Bytes per page of the chip
    #define MAP_BYTES 3840             // Bytes required to make 'node' usage (map) -> (((PAGE_COUNT - PAGE_PER_SECTOR) * BYTES_PER_PAGE) / NODE_SIZE_PARENT) / 8bits
    #define MAP_PAGES 8                // Pages required to hold 'node' usage (map) -> CEILING(MAP_BYTES / BYTES_PER_PAGE)
    #define NODE_PARENT_PER_PAGE 8     // Number of parent nodes per page -> BYTES_PER_PAGE / NODE_SIZE_PARENT
	#define NODE_CHILD_MAX_NODE 6	   // Last valid child node block (due to size of child = size of parent * 2)
	#define NODE_PER_PAGE 4            // Number of nodes per page
    #define BLOCK_COUNT 512            // Number of blocks in the chip
    #define SECTOR_START 1             // The first whole sector number in the chip
    #define SECTOR_END 15              // The last whole sector number in the chip
    #define PAGE_PER_SECTOR 256        // Number of pages per sector in the chip

    #define SECTOR_ERASE_0_SHT_AMT 13  // The shift amount used for a sector zero part erase (see comments below)
    #define SECTOR_ERASE_N_SHT_AMT 18  // The shift amount used for a sector erase (see comments below)
    #define BLOCK_ERASE_SHT_AMT 13     // The shift amount used for a block erase (see comments below)
    #define PAGE_ERASE_SHT_AMT 10      // The shift amount used for a page erase (see comments below)
    #define WRITE_SHT_AMT 10           // The shift amount used for a write operation (see comments below)
    #define READ_OFFSET_SHT_AMT 10     // The shift amount used for a read operation (see comments below)

    // sector erase -> 528size -> OP: 0x7C -> 0a/0b -> 3 address bytes -> 2 D/C, 9 Sector Address (PA11-PA3), 13 D/C -> PA3=0 -> 0a, PA3=1 -> 0b
    // sector erase -> 528size -> OP: 0x7C -> 1/15 -> 3 address bytes -> 2 D/C, 4 Sector Address Bits, 18 D/C
    // sector 0a -> 8 pages, sector 0b -> 248 pages, sector 1/15 -> 256 pages
    // block erase -> 528size -> OP: 0x50 -> 3 address bytes -> 2 D/C, 9 Block Num, 13 D/C -> 512 Blocks
    // page erase -> 528size -> OP: 0x81 -> 3 address bytes -> 2 D/C, 12 Page Num, 10 D/C -> 4096 Pages

    // Write_p1 -> 528size -> MMP to Buffer T -> OP: 0x53 -> 3 address bytes -> 2 D/C, 12 Page Address, 10 D/C
    // write_p2 -> 528size -> MMP PROG T Buffer -> OP: 0x82 -> 3 address bytes -> 2 D/C, 12 Page Address, 10 Buffer Address

    // Read -> 528size -> Low Freq Read -> 0P: 0x03 -> 3 address bytes -> 12 Page Address, 10 Offset, 2 D/C ?
#elif defined(FLASH_CHIP_32M)
    #define MAN_FAM_DEN_VAL 0x27       // Used for Chip Identity (see datasheet)
    #define PAGE_COUNT 8192            // Number of pages in the chip
    #define BYTES_PER_PAGE 528         // Bytes per page of the chip
    #define MAP_BYTES 8064             // Bytes required to make 'node' usage (map) -> (((PAGE_COUNT - PAGE_PER_SECTOR) * BYTES_PER_PAGE) / NODE_SIZE_PARENT) / 8bits
    #define MAP_PAGES 16               // Pages required to hold 'node' usage (map) -> CEILING(MAP_BYTES / BYTES_PER_PAGE)
    #define NODE_PARENT_PER_PAGE 8     // Number of parent nodes per page -> BYTES_PER_PAGE / NODE_SIZE_PARENT
	#define NODE_CHILD_MAX_NODE 6	   // Last valid child node block (due to size of child = size of parent * 2)
	#define NODE_PER_PAGE 4            // Number of nodes per page
    #define BLOCK_COUNT 1024           // Number of blocks in the chip
    #define SECTOR_START 1             // The first whole sector number in the chip
    #define SECTOR_END 63              // The last whole sector number in the chip
    #define PAGE_PER_SECTOR 128        // Number of pages per sector in the chip

    #define SECTOR_ERASE_0_SHT_AMT 13  // The shift amount used for a sector zero part erase (see comments below)
    #define SECTOR_ERASE_N_SHT_AMT 17  // The shift amount used for a sector erase (see comments below)
    #define BLOCK_ERASE_SHT_AMT 13     // The shift amount used for a block erase (see comments below)
    #define PAGE_ERASE_SHT_AMT 10      // The shift amount used for a page erase (see comments below)
    #define WRITE_SHT_AMT 10           // The shift amount used for a write operation (see comments below)
    #define READ_OFFSET_SHT_AMT 10     // The shift amount used for a read operation (see comments below)

    // sector erase -> 528size -> OP: 0x7C -> 0a/0b -> 3 address bytes -> 1 D/C, 10 Sector Address (PA12-PA3), 13 D/C -> PA3=0 -> 0a, PA3=1 -> 0b
    // sector erase -> 528size -> OP: 0x7C -> 1/63 -> 3 address bytes -> 1 D/C, 6 Sector Address Bits, 17 D/C
    // sector 0a -> 8 pages, sector 0b -> 120 pages, sector 1/63 -> 128 pages
    // block erase -> 528size -> OP: 0x50 -> 3 address bytes -> 1 D/C, 10 Block Num, 13 D/C -> 1024 Blocks
    // page erase -> 528size -> OP: 0x81 -> 3 address bytes -> 1 D/C, 13 Page Num, 10 D/C -> 8192 Pages

    // Write_p1 -> 528size -> MMP to Buffer T -> OP: 0x53 -> 3 address bytes -> 1 D/C, 13 Page Address, 10 D/C
    // write_p2 -> 528size -> MMP PROG T Buffer -> OP: 0x82 -> 3 address bytes -> 1 D/C, 13 Page Address, 10 Buffer Address

    // Read -> 528size -> Low Freq Read -> 0P: 0x03 -> 3 address bytes -> 13 Page Address, 10 Offset, 1 D/C ?
#endif

/* General command for AT45DB */
#define FLASH_MANUF_ID                0x1F
#define FLASH_OPCODE_SECTOR_ERASE     0x7C
#define FLASH_OPCODE_BLOCK_ERASE      0x50
#define FLASH_OPCODE_PAGE_ERASE       0x81
#define FLASH_OPCODE_READ_STAT_REG    0xD7
#define FLASH_OPCODE_MAINP_TO_BUF     0x53
#define FLASH_OPCODE_MMP_PROG_TBUF    0x82
#define FLASH_OPCODE_LOWF_READ        0x03
#define FLASH_OPCODE_BUF_WRITE        0x84
#define FLASH_OPCODE_BUF_TO_PAGE      0x83
#define FLASH_OPCODE_READ_DEV_INFO    0x9F
#define FLASH_READY_BITMASK           0x80
#define FLASH_SECTOR_ZER0_A_PAGES     8
#define FLASH_SECTOR_ZERO_A_CODE      0
#define FLASH_SECTOR_ZERO_B_CODE      1

/* Flash parameters page */
#define FLASH_PAGE_MAPPING_NODE_META_DATA  0
#define FLASH_PAGE_MAPPING_NODE_MAP_START  2
#define FLASH_PAGE_MAPPING_NODE_MAP_END    (FLASH_PAGE_MAPPING_NODE_MAP_START + MAP_PAGES)
#define FLASH_PAGE_MAPPING_GFX_START       (FLASH_PAGE_MAPPING_NODE_MAP_END + 1)
#define FLASH_PAGE_MAPPING_GFX_END         (PAGE_PER_SECTOR)
#define FLASH_PAGE_MAPPING_GFX_SIZE        (FLASH_PAGE_MAPPING_GFX_END - FLASH_PAGE_MAPPING_GFX_START)
#define FLASH_BYTES_SIZE          		   ((uint32_t)PAGE_COUNT * (uint32_t)BYTES_PER_PAGE)

// MEMORY_BOUNDARY_CHECKS
void AT45DB_MemoryBoundaryErrorCallback(void);

/*!
 * @brief Function that reads Flash memory chip IDs (Manufacture ID, Device Id (2 bytes), EDI (2 bytes))
 *
 * @return true - successful reading, false - unsuccessful reading
 */
uint8_t AT45DB_CheckID(void);

/*!
 * @brief Function that performs a complete cleaning of the Flash memory chip
 */
void AT45DB_ChipErase(void);

/*!
 * @brief Function that clears the sector of the Flash memory chip (Sector Erase 64 KB)
 *
 * @param SectorNumber		Number sector (Sector 0a, Sector 0b)
 */
void AT45DB_SectorZeroErase(uint8_t SectorNumber);

/*!
 * @brief Function that clears the sector of the Flash memory chip (Sector Erase 64 KB)
 *
 * @param SectorNumber		Number sector
 */
void AT45DB_SectorErase(uint8_t SectorNumber);

/*!
 * @brief Function that clears a single block of Flash memory chip (Block Erase 2 KB)
 *
 * @param BlockNumber		Number of the block
 */
void AT45DB_BlockErase(uint16_t BlockNumber);

/*!
 * @brief Function that clears a single page of the Flash memory chip (Page Erase 256 bytes)
 *
 * @param PageNumber		Number of the page
 */
void AT45DB_PageErase(uint16_t PageNumber);

/*!
 * @brief Function that performs Flash formatting
 */
void AT45DB_FormatFlash(void);

/*!
 * @brief Function that loads a specific page to the internal buffer of the Flash memory chip
 *
 * @param PageNumber		Number of the page
 */
void AT45DB_LoadPageToInternalBuffer(uint16_t PageNumber);

/*!
 * @brief Function that writes to the Flash memory page. Data is entered relative to the offset
 *
 * @param PageNumber		Number of the page
 * @param Offset			Offset relative to the beginning of the page
 * @param Size				The size of the recorded information
 * @param pData				Pointer to an buffer of recorded information
 */
void AT45DB_WriteDataToFlash(uint16_t PageNumber, uint16_t Offset, uint16_t Size, void *pData);

/*!
 * @brief Function that reads from a Flash memory page. Data is read relative to the offset
 *
 * @param PageNumber		Number of the page
 * @param Offset			Offset relative to the beginning of the page
 * @param Size				Size of the information to be read
 * @param pData				Pointer to the buffer where data will be read
 */
void AT45DB_ReadDataFromFlash(uint16_t PageNumber, uint16_t Offset, uint16_t Size, void *pData);

/*!
 * @brief Function that performs continuous data reading
 *
 * @param pData				Pointer to the buffer where data will be read
 * @param Address			Start address
 * @param Size				Size of the information to be read
 */
void AT45DB_FlashRawRead(uint8_t *pData, uint16_t Address, uint16_t Size);

/*!
 * @brief Function that writes data to the internal buffer of the Flash memory chip
 *
 * @param pData				Pointer to the data buffer to be placed in the internal buffer
 * @param Offset			Offset relative to the beginning of the page
 * @param Size				The size of the recorded information
 */
void AT45DB_FlashWriteBuffer(uint8_t* pData, uint16_t Offset, uint16_t Size);

/*!
 * @brief Function that writes the contents of the internal flash memory chip buffer to a Flash memory page
 *
 * @param Page
 */
void AT45DB_WriteBufferToPage(uint16_t Page);

#ifdef __cplusplus
}
#endif
#endif /* FLASH_MEM_H_ */
