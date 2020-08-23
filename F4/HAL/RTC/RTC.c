/*!
 * @file      RTC.c
 *
 * @brief     RTC peripheral implementation
 *
 * @author    Anosov Anton
 */

#include "RTC.h"

/* Private macros */

/* Internal status registers for RTC */
#define RTC_STATUS_REG      			RTC_BKP_DR19
#define RTC_STATUS_TIME_OK              0x1111

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
			  	 uint32_t Format)
{
	uint32_t Status;

	/* Enable PWR peripheral clock */
	__HAL_RCC_PWR_CLK_ENABLE();

	/* Allow access to BKP Domain */
	HAL_PWR_EnableBkUpAccess();

	/* Enable RTC peripheral clock */
	__HAL_RCC_RTC_ENABLE();

	Status = RTC_ReadBackupRegister(RTC_STATUS_REG);

	if(Status == RTC_STATUS_TIME_OK)
	{
		/* Wait for RTC APB registers synchronisation (needed after start-up from Reset) */
		HAL_RTC_WaitForSynchro(pRtc);

		/* Get time */
		HAL_RTC_GetTime(pRtc, pTime, Format);

		/* Get date */
		HAL_RTC_GetDate(pRtc, pDate, Format);

		/* Clear reset flags */
		__HAL_RCC_CLEAR_RESET_FLAGS();

		/* Return OK */
		return 1;
	}
	else
	{
		/* Init peripheral RTC */
		HAL_RTC_Init(pRtc);

		/* Set time for block RTC */
		HAL_RTC_SetTime(pRtc, pTime, Format);

		/* Set date for block RTC */
		HAL_RTC_SetDate(pRtc, pDate, Format);

		/* Save data to backup regiser */
		RTC_WriteBackupRegister(RTC_STATUS_REG, RTC_STATUS_TIME_OK);

		/* RTC was initialized now */
		return 0;
	}
}

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
					 uint32_t Format)
{
	/* Set time */
	HAL_RTC_SetTime(pRtc, pTime, Format);

	/* Set date */
	HAL_RTC_SetDate(pRtc, pDate, Format);

	/* Init RTC */
	HAL_RTC_Init(pRtc);
}

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
		 	 	 	 uint32_t Format)
{
	/* Get time */
	HAL_RTC_GetTime(pRtc, pTime, Format);

	/* Get date */
	HAL_RTC_GetDate(pRtc, pDate, Format);
}

/*!
 * @brief Enable Alarm
 *
 * @param pRtc				Pointer to the RTC_HandleTypeDef description
 * @param pAlarm			Pointer to the RTC_AlarmTypeDef description
 * @param Format			Specifies the format of the entered parameters
 * 							This parameter can be one of the following values: @arg RTC_FORMAT_BIN: Binary data format
 *            																   @arg RTC_FORMAT_BCD: BCD data format
 */
void RTC_EnableAlarm(RTC_HandleTypeDef *pRtc, RTC_AlarmTypeDef *pAlarm, uint32_t Format)
{
	/* Set Alarm */
	HAL_RTC_SetAlarm_IT(pRtc, pAlarm, Format);
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}

/*!
 * @brief Enable WakeUp timer
 *
 * @param pRtc				Pointer to the RTC_HandleTypeDef description
 * @param WakeUp_Counter	WakeUp counter
 * @param WakeUp_Clock		WakeUp Clock
 */
void RTC_EnableWakeUp(RTC_HandleTypeDef *pRtc, uint32_t WakeUp_Counter, uint32_t WakeUp_Clock)
{
	/* Set WakeUp timer */
	HAL_RTCEx_SetWakeUpTimer_IT(pRtc, WakeUp_Counter, WakeUp_Clock);
    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
}

/*!
 * @brief Disables specific alarm
 *
 * @param pRtc				Pointer to the RTC_HandleTypeDef description
 * @param AlarmType			Alarm Specifies the Alarm.
 *          				This parameter can be one of the following values: @arg RTC_ALARM_A
 *            																   @arg RTC_ALARM_B
 */
void RTC_DisableAlarm(RTC_HandleTypeDef *pRtc, uint32_t AlarmType)
{
	HAL_RTC_DeactivateAlarm(pRtc, AlarmType);
}

/*!
 * @brief Disables specific WakeUp timer
 *
 * @param pRtc				Pointer to the RTC_HandleTypeDef description
 */
void RTC_DisableWakeUp(RTC_HandleTypeDef *pRtc)
{
	HAL_RTCEx_DeactivateWakeUpTimer(pRtc);
}

/*!
 * @brief Writes RTC backup register value
 *
 * @param Register			Number register (0 - 19)
 * @param Data				32-bit long value to be stored in RTC backup register
 */
void RTC_WriteBackupRegister(uint8_t Register, uint32_t Data)
{
	uint32_t tmp = 0U;

	/* Check the parameters */
	assert_param(IS_RTC_BKP(BackupRegister));

	tmp = (uint32_t)&(RTC->BKP0R);
	tmp += (Register * 4U);

	/* Write the specified register */
	*(__IO uint32_t*)tmp = (uint32_t)Data;
}

/*!
 * @brief Reads RTC backup register value
 *
 * @param Register			Number register (0 - 19)
 * @return					Value at specific RTC backup register location
 */
uint32_t RTC_ReadBackupRegister(uint8_t Register)
{
	uint32_t tmp = 0U;

	/* Check the parameters */
	assert_param(IS_RTC_BKP(Register));

	tmp = (uint32_t)&(RTC->BKP0R);
	tmp += (Register * 4U);

	/* Read the specified register */
	return (*(__IO uint32_t*)tmp);
}

/* Callbacks */

void RTC_WakeUpHandler(RTC_HandleTypeDef *pRtc)
{
	// IMPLEMENTATION
}

void RTC_AlarmAHandler(RTC_HandleTypeDef *pRtc)
{
	// IMPLEMENTATION
}

void RTC_AlarmBHandler(RTC_HandleTypeDef *pRtc)
{
	// IMPLEMENTATION
}
