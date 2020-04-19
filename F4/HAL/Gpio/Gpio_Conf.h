/*!
 * \file      Gpio_Conf.h
 *
 * \brief     GPIO module implementation
 *
 * \author    Anosov Anton
 */

#ifndef GPIO_CONF_H_
#define GPIO_CONF_H_
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <Gpio_Name_Pins.h>
#include "stm32f4xx.h"

#define BIT_MASK							(0xF0)
#define MCU_PORTA							(0x00)
#define MCU_PORTB							(0x10)
#define MCU_PORTC							(0x20)
#define MCU_PORTD							(0x30)
#define MCU_PORTE							(0x40)
#define MCU_PORTH							(0x50)

/*!
 * Board GPIO pin names
 */
typedef enum GpioPinNames_e
{
	MCU_PINS,
	NC = (int)0xFFFFFFFF
}GpioPinNames_t;

/*!
 * Operation Mode for the GPIO
 */
typedef enum GpioPinModes_e
{
	GPIO_PIN_MODE_INPUT = 0,
	GPIO_PIN_MODE_OUTPUT,
	GPIO_PIN_MODE_AF,
	GPIO_PIN_MODE_ANALOG
}GpioPinModes_t;

/*!
 * Add a pull-up, a pull-down or nothing on the GPIO line
 */
typedef enum GpioPinPulls_e
{
	GPIO_PIN_NO_PULL = 0,
	GPIO_PIN_PULL_UP,
	GPIO_PIN_PULL_DOWN
}GpioPinPulls_t;

/*!
 * Define the GPIO as Push-pull type or Open Drain
 */
typedef enum GpioPinConfigs_e
{
	GPIO_PIN_PUSH_PULL = 0,
	GPIO_PIN_OPEN_DRAIN
}GpioPinConfigs_t;

/*!
 * Define the GPIO IRQ on a rising, falling or both edges
 */
typedef enum GpioPinIrqModes_e
{
	GPIO_PIN_NO_IRQ = 0,
	GPIO_PIN_IRQ_RISING_EDGE,
	GPIO_PIN_IRQ_FALLING_EDGE,
	GPIO_PIN_IRQ_RISING_FALLING_EDGE
}GpioPinIrqModes_t;

/*!
 * Define the IRQ priority on the GPIO
 */
typedef enum GpioPinIrqPriorities_e
{
	GPIO_PIN_IRQ_VERY_HIGH_PRIORITY = 0,
	GPIO_PIN_IRQ_HIGH_PRIORITY,
	GPIO_PIN_IRQ_MEDIUM_PRIORITY,
	GPIO_PIN_IRQ_LOW_PRIORITY,
	GPIO_PIN_IRQ_VERY_LOW_PRIORITY
}GpioPinIrqPriorities_t;

/*!
 * GPIO IRQ handler function prototype
 */
typedef void(GpioIrqHandler)(void *arg);

/*!
 * GPIO Status enum
 */
typedef enum GpioStatus_e
{
    /*!
     * No error occurred
     */
	GPIO_STATUS_OK = 0,

    /*!
     * Error GPIO not init
     */
	GPIO_STATUS_NOT_INIT,

    /*!
     * Error GPIO params
     */
	GPIO_STATUS_ERROR_PARAMS
}GpioStatus_t;

/*!
 * GPIO descriptor
 */
typedef struct GpioDescription_s
{
	GpioPinNames_t			GpioMCU;
	GPIO_TypeDef			*GpioPort;
	uint16_t 	 			GpioPin;
	GpioPinModes_t 	 		GpioPinMode;
	GpioPinPulls_t  		GpioPinPull;
	GpioPinConfigs_t		GpioPinConfig;
	GpioPinIrqModes_t		GpioPinIrq;
	GpioPinIrqPriorities_t	GpioPinIrqPriority;
	GpioIrqHandler			*pGpioIrqCallback;
	void					*arg;
}GpioDescription_t;

/*!
 * \brief Initializing GPIO pin
 *
 * \param[IN] pGpioDescription   	Pointer to the GPIO object
 * \param[IN] GpioPin				Gpio pin
 * \param[IN] GpioMode				Gpio mode
 * \param[IN] GpioConfig			Gpio config
 * \param[IN] GpioPull				Gpio pull
 * \param[IN] State					Gpio state
 * \retval 							Status of the operation
 */
GpioStatus_t GpioInit(GpioDescription_t *pGpioDescription, GpioPinNames_t GpioPin, GpioPinModes_t GpioMode,
					  GpioPinConfigs_t GpioConfig, GpioPinPulls_t GpioPull, uint32_t State);

/*!
 * \brief Toggle GPIO pin
 *
 * \param[IN] pGpioDescription   	Pointer to the GPIO object
 * \retval 							Status of the operation
 */
GpioStatus_t GpioPinToggle(GpioDescription_t *pGpioDescription);

/*!
 * \brief Set GPIO pin
 *
 * \param[IN] pGpioDescription   	Pointer to the GPIO object
 * \retval 							Status of the operation
 */
GpioStatus_t GpioPinSet(GpioDescription_t *pGpioDescription);

/*!
 * \brief Reset GPIO pin
 *
 * \param[IN] pGpioDescription   	Pointer to the GPIO object
 * \retval 							Status of the operation
 */
GpioStatus_t GpioPinReset(GpioDescription_t *pGpioDescription);

/*!
 * \brief Read GPIO pin
 *
 * \param[IN] pGpioDescription   	Pointer to the GPIO object
 * \retval 							Status of the operation
 */
GPIO_PinState GpioPinRead(GpioDescription_t *pGpioDescription);

/*!
 * \brief Set argument for GPIO interrupt
 *
 * \param[IN] pGpioDescription   	Pointer to the GPIO object
 * \param[IN] arg   				Pointer to the GPIO argument
 */
void GpioPinSetArgInterrupt(GpioDescription_t *pGpioDescription, void *arg);

/*!
 * \brief Config interrupt GPIO pin
 *
 * \param[IN] pGpioDescription   	Pointer to the GPIO object
 * \param[IN] GpioIrqMode   		GPIO interrupt mode
 * \param[IN] GpioIrqPriority   	GPIO interrupt priority
 * \param[IN] pGpioIrqHandler   	Pointer to the callback function
 * \param[IN] arg					Pointer to the argument
 * \retval 							Status of the operation
 */
GpioStatus_t GpioPinConfigInterrupt(GpioDescription_t *pGpioDescription, GpioPinIrqModes_t GpioIrqMode, GpioPinIrqPriorities_t GpioIrqPriority,
							     GpioIrqHandler *pGpioIrqHandler, void *arg);

/*!
 * \brief Remove interrupt GPIO pin
 *
 * \param[IN] pGpioDescription   	Pointer to the GPIO object
 * \retval 							Status of the operation
 */
GpioStatus_t GpioPinRemoveInterrupt(GpioDescription_t *pGpioDescription);

#ifdef __cplusplus
}
#endif
#endif /* GPIO_CONF_H_ */
