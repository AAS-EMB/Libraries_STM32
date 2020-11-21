/*!
 * @file      DHT.h
 *
 * @brief     DHT Sensor Library
 *
 * @author    Anosov Anton
 */

#ifndef DHT_H_
#define DHT_H_
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Configuration */
#define DHT_PORT                 GPIOA
#define DHT_PIN                  GPIO_PIN_0
#define DHT_PULLUP               0
#define DHT_TIMEOUT              10000

/*!
 * DHT descriptions
 */
typedef struct DHT_Data_s
{
	float Humidity;
	float Temperature;
} DHT_Data_t;

/*!
 * Enum type sensors DHT
 */
typedef enum DHT_Type_s
{
	DHT11,//!< DHT11
	DHT22 //!< DHT22
} DHT_Type_t;

/*!
 * @brief Get description with DHT Sensor parameters
 *
 * @param Dht     Type sensor
 * @return        DHT_Data_t description
 */
DHT_Data_t DHT_GetData(DHT_Type_t Dht);

#ifdef __cplusplus
}
#endif
#endif /* DHT_H_ */
