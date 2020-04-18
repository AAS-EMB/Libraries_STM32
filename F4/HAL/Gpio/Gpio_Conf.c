/*!
 * \file      Gpio_Conf.c
 *
 * \brief     GPIO module implementation
 *
 * \author    Anosov Anton
 */

#include "Gpio_Conf.h"

static GpioDescription_t *GpioIrq[16];

/*!
 * \brief Initializing GPIO pin
 *
 * \param[IN] pGpioDescription   	Pointer to the GPIO object
 * \param[IN] GpioPin				GPIO pin
 * \param[IN] GpioMode				GPIO mode
 * \param[IN] GpioConfig			GPIO config
 * \param[IN] GpioPull				GPIO pull
 * \param[IN] State					GPIO state
 * \retval 							Status of the operation
 */
GpioStatus_t GpioInit(GpioDescription_t *pGpioDescription, GpioPinNames_t GpioPin, GpioPinModes_t GpioMode,
					  GpioPinConfigs_t GpioConfig, GpioPinPulls_t GpioPull, GPIO_PinState State)
{
	GpioStatus_t ErrCode = GPIO_STATUS_OK;
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	if(GpioPin == NC)
	{
		ErrCode = GPIO_STATUS_ERROR_PARAMS;
		return ErrCode;
	}

	pGpioDescription->GpioMCU = GpioPin;
	pGpioDescription->GpioPin = (0x01 << (GpioPin & 0x0F));
	pGpioDescription->GpioPinMode = GpioMode;
	pGpioDescription->GpioPinConfig = GpioConfig;
	pGpioDescription->GpioPinPull = GpioPull;

	switch(GpioPin & BIT_MASK)
	{
		case MCU_PORTA:
			pGpioDescription->GpioPort = GPIOA;
			__HAL_RCC_GPIOA_CLK_ENABLE();
			break;
		case MCU_PORTB:
			pGpioDescription->GpioPort = GPIOB;
			__HAL_RCC_GPIOB_CLK_ENABLE();
			break;
		case MCU_PORTC:
			pGpioDescription->GpioPort = GPIOC;
			__HAL_RCC_GPIOC_CLK_ENABLE();
			break;
		case MCU_PORTD:
			pGpioDescription->GpioPort = GPIOD;
			__HAL_RCC_GPIOD_CLK_ENABLE();
			break;
		case MCU_PORTE:
			pGpioDescription->GpioPort = GPIOE;
			__HAL_RCC_GPIOE_CLK_ENABLE();
			break;
		case MCU_PORTH:
			pGpioDescription->GpioPort = GPIOH;
			__HAL_RCC_GPIOH_CLK_ENABLE();
			break;
	}

	GPIO_InitStruct.Pin = pGpioDescription->GpioPin;
	GPIO_InitStruct.Pull = pGpioDescription->GpioPinPull;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	switch(GpioMode)
	{
		case GPIO_PIN_MODE_INPUT:
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			break;
		case GPIO_PIN_MODE_OUTPUT:
			if(GpioConfig == GPIO_PIN_OPEN_DRAIN)
				GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
			else
				GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			break;
		case GPIO_PIN_MODE_AF:
			if(GpioConfig == GPIO_PIN_OPEN_DRAIN)
				GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
			else
				GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
			break;
		case GPIO_PIN_MODE_ANALOG:
			GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
			break;
	}

	if(GpioMode == GPIO_PIN_MODE_AF)
		GPIO_InitStruct.Alternate = State;
	else if(GpioMode == GPIO_PIN_MODE_OUTPUT)
	{
		if(State == GPIO_PIN_SET)
			GpioPinSet(pGpioDescription);
		else
			GpioPinReset(pGpioDescription);
	}

	HAL_GPIO_Init(pGpioDescription->GpioPort, &GPIO_InitStruct);

	return ErrCode;
}

/*!
 * \brief Toggle GPIO pin
 *
 * \param[IN] pGpioDescription   	Pointer to the GPIO object
 * \retval 							Status of the operation
 */
GpioStatus_t GpioPinToggle(GpioDescription_t *pGpioDescription)
{
	GpioStatus_t ErrCode = GPIO_STATUS_OK;

	if(pGpioDescription->GpioPort == NULL)
	{
		ErrCode = GPIO_STATUS_NOT_INIT;
		return ErrCode;
	}

	HAL_GPIO_TogglePin(pGpioDescription->GpioPort, pGpioDescription->GpioPin);

	return ErrCode;
}

/*!
 * \brief Set GPIO pin
 *
 * \param[IN] pGpioDescription   	Pointer to the GPIO object
 * \retval 							Status of the operation
 */
GpioStatus_t GpioPinSet(GpioDescription_t *pGpioDescription)
{
	GpioStatus_t ErrCode = GPIO_STATUS_OK;

	if(pGpioDescription->GpioPort == NULL)
	{
		ErrCode = GPIO_STATUS_NOT_INIT;
		return ErrCode;
	}

	HAL_GPIO_WritePin(pGpioDescription->GpioPort, pGpioDescription->GpioPin, GPIO_PIN_SET);

	return ErrCode;
}

/*!
 * \brief Reset GPIO pin
 *
 * \param[IN] pGpioDescription   	Pointer to the GPIO object
 * \retval 							Status of the operation
 */
GpioStatus_t GpioPinReset(GpioDescription_t *pGpioDescription)
{
	GpioStatus_t ErrCode = GPIO_STATUS_OK;

	if(pGpioDescription->GpioPort == NULL)
	{
		ErrCode = GPIO_STATUS_NOT_INIT;
		return ErrCode;
	}

	HAL_GPIO_WritePin(pGpioDescription->GpioPort, pGpioDescription->GpioPin, GPIO_PIN_RESET);

	return ErrCode;
}

/*!
 * \brief Read GPIO pin
 *
 * \param[IN] pGpioDescription   	Pointer to the GPIO object
 * \retval 							GPIO pin state
 */
GPIO_PinState GpioPinRead(GpioDescription_t *pGpioDescription)
{
	return (GPIO_PinState)HAL_GPIO_ReadPin(pGpioDescription->GpioPort, pGpioDescription->GpioPin);
}

/*!
 * \brief Set argument for GPIO interrupt
 *
 * \param[IN] pGpioDescription   	Pointer to the GPIO object
 * \param[IN] arg   				Pointer to the GPIO argument
 */
void GpioPinSetArgInterrupt(GpioDescription_t *pGpioDescription, void *arg)
{
	pGpioDescription->arg = arg;
}

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
							        GpioIrqHandler *pGpioIrqHandler, void *arg)
{
	GpioStatus_t ErrCode = GPIO_STATUS_OK;
    IRQn_Type GpioExtiIrq = EXTI0_IRQn;
    GPIO_InitTypeDef GPIO_InitStruct = {0};

	if(pGpioDescription->GpioPort == NULL)
	{
		ErrCode = GPIO_STATUS_NOT_INIT;
		return ErrCode;
	}
	else if(pGpioIrqHandler == NULL)
	{
		ErrCode = GPIO_STATUS_ERROR_PARAMS;
		return ErrCode;
	}

	pGpioDescription->pGpioIrqCallback = pGpioIrqHandler;
	pGpioDescription->arg = arg;

	switch(GpioIrqMode)
	{
		case GPIO_PIN_IRQ_RISING_EDGE:
			GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
			break;
		case GPIO_PIN_IRQ_FALLING_EDGE:
			GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
			break;
		case GPIO_PIN_IRQ_RISING_FALLING_EDGE:
			GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
			break;
	}

	GPIO_InitStruct.Pin = pGpioDescription->GpioPin;
    GPIO_InitStruct.Pull = pGpioDescription->GpioPinPull;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(pGpioDescription->GpioPort, &GPIO_InitStruct);

    switch(pGpioDescription->GpioPin)
    {
    	case GPIO_PIN_0:
    		GpioExtiIrq = EXTI0_IRQn;
    		break;
    	case GPIO_PIN_1:
    		GpioExtiIrq = EXTI1_IRQn;
    		break;
    	case GPIO_PIN_2:
    		GpioExtiIrq = EXTI2_IRQn;
    		break;
    	case GPIO_PIN_3:
    		GpioExtiIrq = EXTI3_IRQn;
    		break;
    	case GPIO_PIN_4:
    		GpioExtiIrq = EXTI4_IRQn;
        break;
    	case GPIO_PIN_5:
    	case GPIO_PIN_6:
    	case GPIO_PIN_7:
    	case GPIO_PIN_8:
    	case GPIO_PIN_9:
    		GpioExtiIrq = EXTI9_5_IRQn;
    		break;
    	case GPIO_PIN_10:
    	case GPIO_PIN_11:
    	case GPIO_PIN_12:
    	case GPIO_PIN_13:
    	case GPIO_PIN_14:
    	case GPIO_PIN_15:
    		GpioExtiIrq = EXTI15_10_IRQn;
    		break;
    	default:
    		break;
    }

    GpioIrq[(pGpioDescription->GpioMCU) & 0x0F] = pGpioDescription;

    HAL_NVIC_SetPriority(GpioExtiIrq, GpioIrqPriority, 0);
    HAL_NVIC_EnableIRQ(GpioExtiIrq);

    return ErrCode;
}

/*!
 * \brief Remove interrupt GPIO pin
 *
 * \param[IN] pGpioDescription   	Pointer to the GPIO object
 * \retval 							Status of the operation
 */
GpioStatus_t GpioPinRemoveInterrupt(GpioDescription_t *pGpioDescription)
{
	GpioStatus_t ErrCode = GPIO_STATUS_OK;
	IRQn_Type GpioExtiIrq = EXTI0_IRQn;

	if(pGpioDescription->GpioPort == NULL)
	{
		ErrCode = GPIO_STATUS_NOT_INIT;
		return ErrCode;
	}

    switch(pGpioDescription->GpioPin)
    {
    	case GPIO_PIN_0:
    		GpioExtiIrq = EXTI0_IRQn;
    		break;
    	case GPIO_PIN_1:
    		GpioExtiIrq = EXTI1_IRQn;
    		break;
    	case GPIO_PIN_2:
    		GpioExtiIrq = EXTI2_IRQn;
    		break;
    	case GPIO_PIN_3:
    		GpioExtiIrq = EXTI3_IRQn;
    		break;
    	case GPIO_PIN_4:
    		GpioExtiIrq = EXTI4_IRQn;
        break;
    	case GPIO_PIN_5:
    	case GPIO_PIN_6:
    	case GPIO_PIN_7:
    	case GPIO_PIN_8:
    	case GPIO_PIN_9:
    		GpioExtiIrq = EXTI9_5_IRQn;
    		break;
    	case GPIO_PIN_10:
    	case GPIO_PIN_11:
    	case GPIO_PIN_12:
    	case GPIO_PIN_13:
    	case GPIO_PIN_14:
    	case GPIO_PIN_15:
    		GpioExtiIrq = EXTI15_10_IRQn;
    		break;
    	default:
    		break;
    }

    GpioIrq[(pGpioDescription->GpioMCU) & 0x0F] = NULL;

    HAL_NVIC_DisableIRQ(GpioExtiIrq);

    return ErrCode;
}

void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void EXTI1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}

void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}

void EXTI4_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}

void EXTI9_5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
}

void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}

/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GpioPin)
{
	uint8_t CallbackIndex = 0;

	/* Prevent unused argument(s) compilation warning */
	UNUSED(GpioPin);

	/* NOTE: This function Should not be modified, when the callback is needed,
           	   the HAL_GPIO_EXTI_Callback could be implemented in the user file
	 */
	switch(GpioPin)
	{
		case GPIO_PIN_0:
		    if((GpioIrq[CallbackIndex] != NULL) && (GpioIrq[CallbackIndex]->pGpioIrqCallback != NULL))
		        GpioIrq[CallbackIndex]->pGpioIrqCallback(GpioIrq[CallbackIndex]->arg);
		    break;
		case GPIO_PIN_1:
			CallbackIndex = 1;
		    if((GpioIrq[CallbackIndex] != NULL) && (GpioIrq[CallbackIndex]->pGpioIrqCallback != NULL))
		        GpioIrq[CallbackIndex]->pGpioIrqCallback(GpioIrq[CallbackIndex]->arg);
		    break;
		case GPIO_PIN_2:
			CallbackIndex = 2;
		    if((GpioIrq[CallbackIndex] != NULL) && (GpioIrq[CallbackIndex]->pGpioIrqCallback != NULL))
		        GpioIrq[CallbackIndex]->pGpioIrqCallback(GpioIrq[CallbackIndex]->arg);
		    break;
		case GPIO_PIN_3:
			CallbackIndex = 3;
		    if((GpioIrq[CallbackIndex] != NULL) && (GpioIrq[CallbackIndex]->pGpioIrqCallback != NULL))
		        GpioIrq[CallbackIndex]->pGpioIrqCallback(GpioIrq[CallbackIndex]->arg);
		    break;
		case GPIO_PIN_4:
			CallbackIndex = 4;
		    if((GpioIrq[CallbackIndex] != NULL) && (GpioIrq[CallbackIndex]->pGpioIrqCallback != NULL))
		        GpioIrq[CallbackIndex]->pGpioIrqCallback(GpioIrq[CallbackIndex]->arg);
		    break;
		case GPIO_PIN_5:
			CallbackIndex = 5;
		    if((GpioIrq[CallbackIndex] != NULL) && (GpioIrq[CallbackIndex]->pGpioIrqCallback != NULL))
		        GpioIrq[CallbackIndex]->pGpioIrqCallback(GpioIrq[CallbackIndex]->arg);
		    break;
		case GPIO_PIN_6:
			CallbackIndex = 6;
		    if((GpioIrq[CallbackIndex] != NULL) && (GpioIrq[CallbackIndex]->pGpioIrqCallback != NULL))
		        GpioIrq[CallbackIndex]->pGpioIrqCallback(GpioIrq[CallbackIndex]->arg);
		    break;
		case GPIO_PIN_7:
			CallbackIndex = 7;
		    if((GpioIrq[CallbackIndex] != NULL) && (GpioIrq[CallbackIndex]->pGpioIrqCallback != NULL))
		        GpioIrq[CallbackIndex]->pGpioIrqCallback(GpioIrq[CallbackIndex]->arg);
		    break;
		case GPIO_PIN_8:
			CallbackIndex = 8;
		    if((GpioIrq[CallbackIndex] != NULL) && (GpioIrq[CallbackIndex]->pGpioIrqCallback != NULL))
		        GpioIrq[CallbackIndex]->pGpioIrqCallback(GpioIrq[CallbackIndex]->arg);
		    break;
		case GPIO_PIN_9:
			CallbackIndex = 9;
		    if((GpioIrq[CallbackIndex] != NULL) && (GpioIrq[CallbackIndex]->pGpioIrqCallback != NULL))
		        GpioIrq[CallbackIndex]->pGpioIrqCallback(GpioIrq[CallbackIndex]->arg);
		    break;
		case GPIO_PIN_10:
			CallbackIndex = 10;
		    if((GpioIrq[CallbackIndex] != NULL) && (GpioIrq[CallbackIndex]->pGpioIrqCallback != NULL))
		        GpioIrq[CallbackIndex]->pGpioIrqCallback(GpioIrq[CallbackIndex]->arg);
		    break;
		case GPIO_PIN_11:
			CallbackIndex = 11;
		    if((GpioIrq[CallbackIndex] != NULL) && (GpioIrq[CallbackIndex]->pGpioIrqCallback != NULL))
		        GpioIrq[CallbackIndex]->pGpioIrqCallback(GpioIrq[CallbackIndex]->arg);
		    break;
		case GPIO_PIN_12:
			CallbackIndex = 12;
		    if((GpioIrq[CallbackIndex] != NULL) && (GpioIrq[CallbackIndex]->pGpioIrqCallback != NULL))
		        GpioIrq[CallbackIndex]->pGpioIrqCallback(GpioIrq[CallbackIndex]->arg);
		    break;
		case GPIO_PIN_13:
			CallbackIndex = 13;
		    if((GpioIrq[CallbackIndex] != NULL) && (GpioIrq[CallbackIndex]->pGpioIrqCallback != NULL))
		        GpioIrq[CallbackIndex]->pGpioIrqCallback(GpioIrq[CallbackIndex]->arg);
		    break;
		case GPIO_PIN_14:
			CallbackIndex = 14;
		    if((GpioIrq[CallbackIndex] != NULL) && (GpioIrq[CallbackIndex]->pGpioIrqCallback != NULL))
		        GpioIrq[CallbackIndex]->pGpioIrqCallback(GpioIrq[CallbackIndex]->arg);
		    break;
		case GPIO_PIN_15:
			CallbackIndex = 15;
		    if((GpioIrq[CallbackIndex] != NULL) && (GpioIrq[CallbackIndex]->pGpioIrqCallback != NULL))
		        GpioIrq[CallbackIndex]->pGpioIrqCallback(GpioIrq[CallbackIndex]->arg);
		    break;
	}
}
