/*!
 * @file      DHT.c
 *
 * @brief     DHT Sensor Library
 *
 * @author    Anosov Anton
 */

#include "DHT.h"

/*!
 * @brief Set line in down
 */
static inline void DHT_SetLineDown(void)
{
	DHT_PORT->BSRR = (uint32_t)DHT_PIN << 16U;
}

/*!
 * @brief Set line in up
 */
static inline void DHT_SetLineUp(void)
{
	DHT_PORT->BSRR = DHT_PIN;
}

/*!
 * @brief Get state line
 *
 * @return        0 - Line in down, 1 - Line in up
 */
static inline uint8_t DHT_GetStateLine(void)
{
	uint8_t Result;
	Result = (HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN) == GPIO_PIN_SET) ? 1 : 0;
	return Result;
}

/*!
 * @brief Set as pin in output mode
 */
static void GpioSetAsOutput(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	DHT_SetLineUp();

	GPIO_InitStruct.Pin = DHT_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
#if DHT_PULLUP == 1
	GPIO_InitStruct.Pull = GPIO_PULLUP;
#else
	GPIO_InitStruct.Pull = GPIO_NOPULL;
#endif
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(DHT_PORT, &GPIO_InitStruct);
}

/*!
 * @brief Set as pin in input mode
 */
static void GpioSetAsInput(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = DHT_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
#if DHT_PULLUP == 1
	GPIO_InitStruct.Pull = GPIO_PULLUP;
#else
	GPIO_InitStruct.Pull = GPIO_NOPULL;
#endif
	HAL_GPIO_Init(DHT_PORT, &GPIO_InitStruct);
}

/*!
 * @brief Delay in ms
 * @param ms      time in ms
 */
static inline void DHT_Delay(uint32_t ms)
{
	HAL_Delay(ms);
}

/*!
 * @brief Get description with DHT Sensor parameters
 *
 * @param Dht     Type sensor
 * @return        DHT_Data_t description
 */
DHT_Data_t DHT_GetData(DHT_Type_t Dht)
{
	DHT_Data_t Data = {0.0f};

	GpioSetAsOutput();
	DHT_SetLineDown();
	DHT_Delay(15);
	DHT_SetLineUp();
	GpioSetAsInput();

	uint16_t timeout = 0;
	while(DHT_GetStateLine())
	{
		timeout++;
		if (timeout > DHT_TIMEOUT) return Data;
	}
	timeout = 0;
	while(!DHT_GetStateLine())
	{
		timeout++;
		if (timeout > DHT_TIMEOUT) return Data;
	}
	timeout = 0;
	while(DHT_GetStateLine())
	{
		timeout++;
		if (timeout > DHT_TIMEOUT) return Data;
	}

	uint8_t RawData[5] = {0};
	for(uint8_t i = 0; i < 5; i++)
	{
		for(uint8_t j = 7; j != 255; j--)
		{
			uint32_t hT = 0, lT = 0;
			while(!DHT_GetStateLine()) lT++;
			while(DHT_GetStateLine())  hT++;
			if(hT > lT) RawData[i] |= (1 << j);
		}
	}

	if((uint8_t)(RawData[0] + RawData[1] + RawData[2] + RawData[3]) == RawData[4])
	{
		if (Dht == DHT22)
		{
			Data.Humidity = (float)(((uint16_t)RawData[0] << 8) | RawData[1]) * 0.1f;
			if(!(RawData[2] & (1 << 7))) {
				Data.Temperature = (float)(((uint16_t)RawData[2] << 8) | RawData[3]) * 0.1f;
			} else {
				RawData[2] &= ~(1 << 7);
				Data.Temperature = (float)(((uint16_t)RawData[2] << 8) | RawData[3]) * (-0.1f);
			}
		}
		else if (Dht == DHT11)
		{
			Data.Humidity    = (float)RawData[0];
			Data.Temperature = (float)RawData[2];
		}
	}

	return Data;
}
