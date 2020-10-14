/*!
 * @file		W25Qxx_Conf.h
 *
 * @brief 		Configuration header file for work interface SPI with W25Qxx
 *
 * @author		Anosov Anton
 */

#ifndef EXTERNAL_FLASH_W25QXX_W25QXX_CONF_H_
#define EXTERNAL_FLASH_W25QXX_W25QXX_CONF_H_
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <stdint.h>
#include <string.h>

extern SPI_HandleTypeDef hspi1;

#define W25Qxx_BEGIN_CRITICAL_SECTION()				__disable_irq()
#define W25Qxx_END_CRITICAL_SECTION()				__enable_irq()

/* Configuration */
#define W25Qxx_SPI						hspi1
#define W25Qxx_CS_PORT						GPIOB
#define W25Qxx_CS_PIN						GPIO_PIN_0
#define W25Qxx_CHIP_ENABLE()					HAL_GPIO_WritePin(W25Qxx_CS_PORT, W25Qxx_CS_PIN, GPIO_PIN_RESET)
#define W25Qxx_CHIP_DISABLE()					HAL_GPIO_WritePin(W25Qxx_CS_PORT, W25Qxx_CS_PIN, GPIO_PIN_SET)

#if defined(USE_FREERTOS)
	#include "cmsis_os.h"
	#define W25Qxx_DELAY(ms)				osDelay(ms)
#else
	#define W25Qxx_DELAY(ms)				HAL_Delay(ms)
#endif

#ifdef __cplusplus
}
#endif
#endif /* EXTERNAL_FLASH_W25QXX_W25QXX_CONF_H_ */
