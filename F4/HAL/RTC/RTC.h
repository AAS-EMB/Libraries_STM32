/*!
 * @file      RTC.h
 *
 * @brief     RTC peripheral implementation
 *
 * @author    Anosov Anton
 */

#ifndef RTC_H_
#define RTC_H_
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/*!
 * @brief Init RTC peripheral
 *
 * @param pRtc				Pointer to the RTC_HandleTypeDef description
 * @param pDate				Pointer to the RTC_DateTypeDef description
 * @param pTime				Pointer to the RTC_TimeTypeDef description
 * @param Format			Specifies the format of the entered parameters
 * 							This parameter can be one of the following values: @arg RTC_FORMAT_BIN: Binary data format
 *            																   @arg RTC_FORMAT_BCD: BCD data format
 * @return					Status operation (0 - RTC was now initialized first time, 1 -  RTC has already been initialized and time is set)
 */
uint8_t RTC_Init(RTC_HandleTypeDef *pRtc, RTC_DateTypeDef *pDate, RTC_TimeTypeDef *pTime, \
 			  	 uint32_t Format);

/*!
 * @brief Sets date and time to internal RTC registers
 *
 * @param pRtc				Pointer to the RTC_HandleTypeDef description
 * @param pDate				Pointer to the RTC_DateTypeDef description
 * @param pTime				Pointer to the RTC_TimeTypeDef description
 * @param Format			Specifies the format of the entered parameters
 * 							This parameter can be one of the following values: @arg RTC_FORMAT_BIN: Binary data format
 *            																   @arg RTC_FORMAT_BCD: BCD data format
 */
void RTC_SetDateTime(RTC_HandleTypeDef *pRtc, RTC_DateTypeDef *pDate, RTC_TimeTypeDef *pTime,
					 uint32_t Format);

/*!
 * @brief Gets date and time to internal RTC registers
 *
 * @param pRtc				Pointer to the RTC_HandleTypeDef description
 * @param pDate				Pointer to the RTC_DateTypeDef description
 * @param pTime				Pointer to the RTC_TimeTypeDef description
 * @param Format			Specifies the format of the entered parameters
 * 							This parameter can be one of the following values: @arg RTC_FORMAT_BIN: Binary data format
 *            																   @arg RTC_FORMAT_BCD: BCD data format
 */
void RTC_GetDateTime(RTC_HandleTypeDef *pRtc, RTC_DateTypeDef *pDate, RTC_TimeTypeDef *pTime,
		 	 	 	 uint32_t Format);

/*!
 * @brief Enable Alarm
 *
 * @param pRtc				Pointer to the RTC_HandleTypeDef description
 * @param pAlarm			Pointer to the RTC_AlarmTypeDef description
 * @param Format			Specifies the format of the entered parameters
 * 							This parameter can be one of the following values: @arg RTC_FORMAT_BIN: Binary data format
 *            																   @arg RTC_FORMAT_BCD: BCD data format
 */
void RTC_EnableAlarm(RTC_HandleTypeDef *pRtc, RTC_AlarmTypeDef *pAlarm, uint32_t Format);

/*!
 * @brief Enable WakeUp timer
 *
 * @param pRtc				Pointer to the RTC_HandleTypeDef description
 * @param WakeUp_Counter	WakeUp counter
 * @param WakeUp_Clock		WakeUp Clock
 */
void RTC_EnableWakeUp(RTC_HandleTypeDef *pRtc, uint32_t WakeUp_Counter, uint32_t WakeUp_Clock);

/*!
 * @brief Disables specific alarm
 *
 * @param pRtc				Pointer to the RTC_HandleTypeDef description
 * @param AlarmType			Alarm Specifies the Alarm.
 *          				This parameter can be one of the following values: @arg RTC_ALARM_A
 *            																   @arg RTC_ALARM_B
 */
void RTC_DisableAlarm(RTC_HandleTypeDef *pRtc, uint32_t AlarmType);

/*!
 * @brief Disables specific WakeUp timer
 *
 * @param pRtc				Pointer to the RTC_HandleTypeDef description
 */
void RTC_DisableWakeUp(RTC_HandleTypeDef *pRtc);

/*!
 * @brief Writes RTC backup register value
 *
 * @param Register			Number register (0 - 19)
 * @param Data				32-bit long value to be stored in RTC backup register
 */
void RTC_WriteBackupRegister(uint8_t Register, uint32_t Data);

/*!
 * @brief Reads RTC backup register value
 *
 * @param Register			Number register (0 - 19)
 * @return					Value at specific RTC backup register location
 */
uint32_t RTC_ReadBackupRegister(uint8_t Register);

/* Callbacks */
void RTC_WakeUpHandler(RTC_HandleTypeDef *pRtc);
void RTC_AlarmAHandler(RTC_HandleTypeDef *pRtc);
void RTC_AlarmBHandler(RTC_HandleTypeDef *pRtc);

#ifdef __cplusplus
}
#endif
#endif /* RTC_H_ */
