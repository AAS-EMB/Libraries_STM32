/*!
 * @file      CAN.h
 *
 * @brief     CAN bus module
 *
 * @author    Anosov Anton
 */

#ifndef CAN_H_
#define CAN_H_
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/*!
 * Standard filter description
 */
typedef struct CAN_FilterStdId_s
{
	/*!
	 * Number of bank (range 0 - 13)
	 */
	uint32_t FilterBank;

	/*!
	 * Filter scale (@arg CAN_FILTERSCALE_16BIT, @arg CAN_FILTERSCALE_32BIT)
	 */
	uint32_t FilterScale;

	/*!
	 * Standard ID high
	 */
	uint16_t IdHigh;

	/*!
	 * Standard ID mask high
	 */
	uint16_t IdHighMask;

	/*!
	 * Standard ID low
	 */
	uint16_t IdLow;

	/*!
	 * Standard ID mask low
	 */
	uint16_t IdLowMask;
}CAN_FilterStdId_t;

/*!
 * Extended filter description
 */
typedef struct CAN_FilterExtId_s
{
	/*!
	 * Number of bank (range 0 - 13)
	 */
	uint32_t FilterBank;

	/*!
	 * Standard ID high
	 */
	uint32_t IdHigh;

	/*!
	 * Standard ID mask high
	 */
	uint32_t IdHighMask;

	/*!
	 * Standard ID low
	 */
	uint32_t IdLow;

	/*!
	 * Standard ID mask low
	 */
	uint32_t IdLowMask;
}CAN_FilterExtId_t;

/*!
 * Message description
 */
typedef struct CAN_Message_s
{
	/*!
	 * Standard ID
	 */
	uint16_t StdID;

	/*!
	 * Extended ID
	 */
	uint32_t ExtID;

	/*!
	 * Tx buffer
	 */
	uint8_t TxData[8];

	/*!
	 * Tx buffer size
	 */
	uint8_t SizeMsgTx;

	/*!
	 * Rx buffer
	 */
	uint8_t RxData[8];

	/*!
	 * Rx buffer size
	 */
	uint8_t SizeMsgRx;
}CAN_Message_t;

/*!
 * @brief Initial CAN bus
 *
 * @param pCanHandle		Pointer to the CAN_HandleTypeDef description
 */
void CAN_Init(CAN_HandleTypeDef *pCanHandle);

/*!
 * @brief DeInit CAN bus
 *
 * @param pCanHandle		Pointer to the CAN_HandleTypeDef description
 */
void CAN_DeInit(CAN_HandleTypeDef *pCanHandle);

/*!
 * @brief Start CAN
 *
 * @param pCanHandle		Pointer to the CAN_HandleTypeDef description
 */
void CAN_Start(CAN_HandleTypeDef *pCanHandle);

/*!
 * @brief Stop CAN
 *
 * @param pCanHandle		Pointer to the CAN_HandleTypeDef description
 */
void CAN_Stop(CAN_HandleTypeDef *pCanHandle);

/*!
 * @brief Add a range of filtered standard IDs (11 bits)
 *
 * @param pCanHandle		Pointer to the CAN_HandleTypeDef description
 * @param pCanFilter		Pointer to the CAN_FilterStdId_t description
 */
void CAN_AddRangeFilterStdID(CAN_HandleTypeDef *pCanHandle, CAN_FilterStdId_t *pCanFilter);

/*!
 * @brief Add a range of filtered extended IDs (29 bits)
 *
 * @param pCanHandle		Pointer to the CAN_HandleTypeDef description
 * @param pCanFilter		Pointer to the CAN_FilterStdId_t description
 */
void CAN_AddRangeFilterExtID(CAN_HandleTypeDef *pCanHandle, CAN_FilterExtId_t *pCanFilter);

/*!
 * @brief Adding standard IDs (11 bits) to the filter
 *
 * @param pCanHandle		Pointer to the CAN_HandleTypeDef description
 * @param pCanFilter		Pointer to the CAN_FilterStdId_t description
 */
void CAN_AddFilterStdID(CAN_HandleTypeDef *pCanHandle, CAN_FilterStdId_t *pCanFilter);

/*!
 * @brief Adding extended IDs (29 bits) to the filter
 *
 * @param pCanHandle		Pointer to the CAN_HandleTypeDef description
 * @param pCanFilter		Pointer to the CAN_FilterStdId_t description
 */
void CAN_AddFilterExtID(CAN_HandleTypeDef *pCanHandle, CAN_FilterStdId_t *pCanFilter);

/*!
 * @brief Send message with standard ID
 *
 * @param pCanHandle		Pointer to the CAN_HandleTypeDef description
 */
void CAN_StdSendMessage(CAN_HandleTypeDef *pCanHandle);

/*!
 * @brief Send message with extended ID
 *
 * @param pCanHandle		Pointer to the CAN_HandleTypeDef description
 */
void CAN_ExtSendMessage(CAN_HandleTypeDef *pCanHandle);

#ifdef __cplusplus
}
#endif
#endif /* CAN_H_ */
