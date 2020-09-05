/*!
 * @file      CAN.c
 *
 * @brief     CAN bus module
 *
 * @author    Anosov Anton
 */

#include "CAN.h"

#define CAN_IDE_32            0b00000100
#define CAN_FREE_LEVEL		  3

static uint32_t RCC_CAN1_CLK_ENABLED = 0;
static CAN_TxHeaderTypeDef TxHeader;
static CAN_RxHeaderTypeDef RxHeader;
static uint32_t TxMailbox;
CAN_Message_t Msg;

/*!
 * @brief Initial CAN bus
 *
 * @param pCanHandle		Pointer to the CAN_HandleTypeDef description
 */
void CAN_Init(CAN_HandleTypeDef *pCanHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	if(pCanHandle->Instance == CAN1)
	{
	    /* CAN1 clock enable */
		RCC_CAN1_CLK_ENABLED++;
		if(RCC_CAN1_CLK_ENABLED == 1)
		{
			__HAL_RCC_CAN1_CLK_ENABLE();
		}

	    __HAL_RCC_GPIOA_CLK_ENABLE();
	    /**CAN1 GPIO Configuration
	    PA11     ------> CAN1_RX
	    PA12     ------> CAN1_TX
	    */
	    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
	    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	    /* CAN1 interrupt Init */
	    HAL_NVIC_SetPriority(CAN1_TX_IRQn, 0, 0);
	    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
	    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
	    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
	    HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 0, 0);
	    HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
	}
	else if(pCanHandle->Instance == CAN2)
	{
	    /* CAN2 clock enable */
		RCC_CAN1_CLK_ENABLED++;
		if(RCC_CAN1_CLK_ENABLED == 1)
		{
			__HAL_RCC_CAN2_CLK_ENABLE();
		}

	    __HAL_RCC_GPIOB_CLK_ENABLE();
	    /**CAN2 GPIO Configuration
	    PB5     ------> CAN2_RX
	    PB6     ------> CAN2_TX
	    */
	    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = GPIO_AF9_CAN2;
	    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	    /* CAN2 interrupt Init */
	    HAL_NVIC_SetPriority(CAN2_TX_IRQn, 0, 0);
	    HAL_NVIC_EnableIRQ(CAN2_TX_IRQn);
	    HAL_NVIC_SetPriority(CAN2_RX0_IRQn, 0, 0);
	    HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);
	    HAL_NVIC_SetPriority(CAN2_SCE_IRQn, 0, 0);
	    HAL_NVIC_EnableIRQ(CAN2_SCE_IRQn);
	}

	if (HAL_CAN_Init(pCanHandle) != HAL_OK)
	{
		CAN_ErrorHandler();
	}
}

/*!
 * @brief DeInit CAN bus
 *
 * @param pCanHandle		Pointer to the CAN_HandleTypeDef description
 */
void CAN_DeInit(CAN_HandleTypeDef *pCanHandle)
{
	if(pCanHandle->Instance == CAN1)
	{
	    /* Peripheral clock disable */
	    RCC_CAN1_CLK_ENABLED--;
	    if(RCC_CAN1_CLK_ENABLED == 0)
	    {
	    	__HAL_RCC_CAN1_CLK_DISABLE();
	    }

	    /**CAN1 GPIO Configuration
	    PA11     ------> CAN1_RX
	    PA12     ------> CAN1_TX
	    */
	    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

	    /* CAN1 interrupt Deinit */
	    HAL_NVIC_DisableIRQ(CAN1_TX_IRQn);
	    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
	    HAL_NVIC_DisableIRQ(CAN1_SCE_IRQn);
	}
	else if(pCanHandle->Instance == CAN2)
	{
	    /* Peripheral clock disable */
	    __HAL_RCC_CAN2_CLK_DISABLE();
	    RCC_CAN1_CLK_ENABLED--;
	    if(RCC_CAN1_CLK_ENABLED == 0)
	    {
	    	__HAL_RCC_CAN1_CLK_DISABLE();
	    }

	    /**CAN2 GPIO Configuration
	    PB5     ------> CAN2_RX
	    PB6     ------> CAN2_TX
	    */
	    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_5|GPIO_PIN_6);

	    /* CAN2 interrupt Deinit */
	    HAL_NVIC_DisableIRQ(CAN2_TX_IRQn);
	    HAL_NVIC_DisableIRQ(CAN2_RX0_IRQn);
	    HAL_NVIC_DisableIRQ(CAN2_SCE_IRQn);
	}

	if (HAL_CAN_DeInit(pCanHandle) != HAL_OK)
	{
		CAN_ErrorHandler();
	}
}

/*!
 * @brief Start CAN
 *
 * @param pCanHandle		Pointer to the CAN_HandleTypeDef description
 */
void CAN_Start(CAN_HandleTypeDef *pCanHandle)
{
	if(HAL_CAN_Start(pCanHandle) != HAL_OK)
	{
		/* Start CAN Error */
		CAN_ErrorHandler();
	}
	if(HAL_CAN_ActivateNotification(pCanHandle, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	{
		/* Activate notification CAN Error */
		CAN_ErrorHandler();
	}
}

/*!
 * @brief Stop CAN
 *
 * @param pCanHandle		Pointer to the CAN_HandleTypeDef description
 */
void CAN_Stop(CAN_HandleTypeDef *pCanHandle)
{
	if(HAL_CAN_Stop(pCanHandle) != HAL_OK)
	{
		/* Start CAN Error */
		CAN_ErrorHandler();
	}
	if(HAL_CAN_DeactivateNotification(pCanHandle, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	{
		/* Activate notification CAN Error */
		CAN_ErrorHandler();
	}
}

/*!
 * @brief Add a range of filtered standard IDs (11 bits)
 *
 * @param pCanHandle		Pointer to the CAN_HandleTypeDef description
 * @param pCanFilter		Pointer to the CAN_FilterStdId_t description
 */
void CAN_AddRangeFilterStdID(CAN_HandleTypeDef *pCanHandle, CAN_FilterStdId_t *pCanFilter)
{
	CAN_FilterTypeDef CanFilterConfig;

	/* Number of the bank filter (0 - 13) */
	CanFilterConfig.FilterBank = pCanFilter->FilterBank;

	/* Mode filter */
	CanFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;

	/* Scale filter */
	CanFilterConfig.FilterScale = pCanFilter->FilterScale;

	/* Check scale filter */
	if(pCanFilter->FilterScale == CAN_FILTERSCALE_16BIT)
	{
		/* ID of filter number 1 */
		CanFilterConfig.FilterIdHigh = (pCanFilter->IdHigh << 5);

		/* ID of filter number 2 */
		CanFilterConfig.FilterIdLow = (pCanFilter->IdLow << 5);

		/* Filter mask number 1 */
		CanFilterConfig.FilterMaskIdHigh = (pCanFilter->IdHighMask << 5);

		/* Filter mask number 2 */
		CanFilterConfig.FilterMaskIdLow = (pCanFilter->IdLowMask << 5);
	}
	else if(pCanFilter->FilterScale == CAN_FILTERSCALE_32BIT)
	{
		/* Older part of the filter */
		CanFilterConfig.FilterIdHigh = (pCanFilter->IdHigh << 5);

		/* Younger part of the filter */
		CanFilterConfig.FilterIdLow = 0x0000;

		/* Older part of the mask */
		CanFilterConfig.FilterMaskIdHigh = (pCanFilter->IdHighMask << 5);

		/* Younger part of the mask */
		CanFilterConfig.FilterMaskIdLow = 0x0000;
	}

	/* Input FIFO */
	CanFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;

	/* Filter activation */
	CanFilterConfig.FilterActivation = ENABLE;
	CanFilterConfig.SlaveStartFilterBank = 14;

	/* Configuration filter */
	if(HAL_CAN_ConfigFilter(pCanHandle, &CanFilterConfig) != HAL_OK)
	{
		CAN_ErrorHandler();
	}
}

/*!
 * @brief Add a range of filtered extended IDs (29 bits)
 *
 * @param pCanHandle		Pointer to the CAN_HandleTypeDef description
 * @param pCanFilter		Pointer to the CAN_FilterStdId_t description
 */
void CAN_AddRangeFilterExtID(CAN_HandleTypeDef *pCanHandle, CAN_FilterExtId_t *pCanFilter)
{
	CAN_FilterTypeDef CanFilterConfig;

	/* Number of the bank filter (0 - 13) */
	CanFilterConfig.FilterBank = pCanFilter->FilterBank;

	/* Mode filter */
	CanFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;

	/* Scale filter */
	CanFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;

	/* Older part of the filter */
	CanFilterConfig.FilterIdHigh = (uint16_t)(pCanFilter->IdHigh >> 13);

	/* Younger part of the filter */
	CanFilterConfig.FilterIdLow = (uint16_t)(pCanFilter->IdLow << 3) | CAN_IDE_32;

	/* Older part of the mask */
	CanFilterConfig.FilterMaskIdHigh = (uint16_t)(pCanFilter->IdHighMask >> 13);

	/* Younger part of the mask */
	CanFilterConfig.FilterMaskIdLow = (uint16_t)(pCanFilter->IdLowMask << 3) | CAN_IDE_32;

	/* Input FIFO */
	CanFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;

	/* Filter activation */
	CanFilterConfig.FilterActivation = ENABLE;
	CanFilterConfig.SlaveStartFilterBank = 14;

	/* Configuration filter */
	if(HAL_CAN_ConfigFilter(pCanHandle, &CanFilterConfig) != HAL_OK)
	{
		CAN_ErrorHandler();
	}
}

/*!
 * @brief Adding standard IDs (11 bits) to the filter
 *
 * @param pCanHandle		Pointer to the CAN_HandleTypeDef description
 * @param pCanFilter		Pointer to the CAN_FilterStdId_t description
 */
void CAN_AddFilterStdID(CAN_HandleTypeDef *pCanHandle, CAN_FilterStdId_t *pCanFilter)
{
	CAN_FilterTypeDef CanFilterConfig;

	/* Number of the bank filter (0 - 13) */
	CanFilterConfig.FilterBank = pCanFilter->FilterBank;

	/* Mode filter */
	CanFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;

	/* Scale filter */
	CanFilterConfig.FilterScale = pCanFilter->FilterScale;

	/* Check scale filter */
	if(pCanFilter->FilterScale == CAN_FILTERSCALE_16BIT)
	{
		/* ID1 */
		CanFilterConfig.FilterIdHigh = (pCanFilter->IdHigh << 5);

		/* ID2 */
		CanFilterConfig.FilterIdLow = (pCanFilter->IdLow << 5);

		/* ID3 */
		CanFilterConfig.FilterMaskIdHigh = (pCanFilter->IdHighMask << 5);

		/* ID4 */
		CanFilterConfig.FilterMaskIdLow = (pCanFilter->IdLowMask << 5);
	}
	else if(pCanFilter->FilterScale == CAN_FILTERSCALE_32BIT)
	{
		/* Older part of the ID1 */
		CanFilterConfig.FilterIdHigh = (pCanFilter->IdHigh << 5);

		/* Younger part of the ID1 */
		CanFilterConfig.FilterIdLow = 0x0000;

		/* Older part of the ID2 */
		CanFilterConfig.FilterMaskIdHigh = (pCanFilter->IdHighMask << 5);

		/* Younger part of the ID2 */
		CanFilterConfig.FilterMaskIdLow = 0x0000;
	}

	/* Input FIFO */
	CanFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;

	/* Filter activation */
	CanFilterConfig.FilterActivation = ENABLE;
	CanFilterConfig.SlaveStartFilterBank = 14;

	/* Configuration filter */
	if(HAL_CAN_ConfigFilter(pCanHandle, &CanFilterConfig) != HAL_OK)
	{
		CAN_ErrorHandler();
	}
}

/*!
 * @brief Adding extended IDs (29 bits) to the filter
 *
 * @param pCanHandle		Pointer to the CAN_HandleTypeDef description
 * @param pCanFilter		Pointer to the CAN_FilterStdId_t description
 */
void CAN_AddFilterExtID(CAN_HandleTypeDef *pCanHandle, CAN_FilterStdId_t *pCanFilter)
{
	CAN_FilterTypeDef CanFilterConfig;

	/* Number of the bank filter (0 - 13) */
	CanFilterConfig.FilterBank = pCanFilter->FilterBank;

	/* Mode filter */
	CanFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;

	/* Scale filter */
	CanFilterConfig.FilterScale = pCanFilter->FilterScale;

	/* Older part of the filter */
	CanFilterConfig.FilterIdHigh = (uint16_t)(pCanFilter->IdHigh >> 13);

	/* Younger part of the filter */
	CanFilterConfig.FilterIdLow = (uint16_t)(pCanFilter->IdLow << 3) | CAN_IDE_32;

	/* Older part of the mask */
	CanFilterConfig.FilterMaskIdHigh = (uint16_t)(pCanFilter->IdHighMask >> 13);

	/* Younger part of the mask */
	CanFilterConfig.FilterMaskIdLow = (uint16_t)(pCanFilter->IdLowMask << 3) | CAN_IDE_32;

	/* Input FIFO */
	CanFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;

	/* Filter activation */
	CanFilterConfig.FilterActivation = ENABLE;
	CanFilterConfig.SlaveStartFilterBank = 14;

	/* Configuration filter */
	if(HAL_CAN_ConfigFilter(pCanHandle, &CanFilterConfig) != HAL_OK)
	{
		CAN_ErrorHandler();
	}
}

/*!
 * @brief Send message with standard ID
 *
 * @param pCanHandle		Pointer to the CAN_HandleTypeDef description
 */
void CAN_StdSendMessage(CAN_HandleTypeDef *pCanHandle)
{
	TxHeader.StdId = Msg.StdID;
	TxHeader.ExtId = 0x00;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.DLC = Msg.SizeMsgTx;
	TxHeader.TransmitGlobalTime = DISABLE;

	/* Request transmission */
	if(HAL_CAN_AddTxMessage(pCanHandle, &TxHeader, Msg.TxData, &TxMailbox) != HAL_OK)
	{
		/* Transmission request Error */
		CAN_ErrorHandler();
	}

	/* Wait transmission complete */
	while(HAL_CAN_GetTxMailboxesFreeLevel(pCanHandle) != CAN_FREE_LEVEL) {}
}

/*!
 * @brief Send message with extended ID
 *
 * @param pCanHandle		Pointer to the CAN_HandleTypeDef description
 */
void CAN_ExtSendMessage(CAN_HandleTypeDef *pCanHandle)
{
	TxHeader.StdId = 0x00;
	TxHeader.ExtId = Msg.ExtID;
	TxHeader.IDE = CAN_ID_EXT;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.DLC = Msg.SizeMsgTx;
	TxHeader.TransmitGlobalTime = DISABLE;

	/* Request transmission */
	if(HAL_CAN_AddTxMessage(pCanHandle, &TxHeader, Msg.TxData, &TxMailbox) != HAL_OK)
	{
		/* Transmission request Error */
		CAN_ErrorHandler();
	}

	/* Wait transmission complete */
	while(HAL_CAN_GetTxMailboxesFreeLevel(pCanHandle) != CAN_FREE_LEVEL) {}
}

// Callback
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	/* Get message from mailbox */
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, Msg.RxData);
	if (RxHeader.IDE == CAN_ID_EXT)
	{
		Msg.ExtID = RxHeader.ExtId;
	}
	else
	{
		Msg.StdID = RxHeader.StdId;
	}
}

void CAN_ErrorHandler(void)
{
	NVIC_SystemReset();
}
