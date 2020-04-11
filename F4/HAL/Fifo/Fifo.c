/*!
 * \file      Fifo.c
 *
 * \brief     FIFO buffer implementation
 *
 * \author    Anosov Anton
 */

#include "Fifo.h"

/*!
 * \brief Checks if the free number of bytes
 *
 * \param[IN] pFifo   	Pointer to the FIFO object
 * \parem[IN] Lenght	Data lenght
 * \retval 				Status of the operation
 */
static FifoStatus_t IsFifoFreeSize(Fifo_t *pFifo, uint16_t Lenght)
{
	FifoStatus_t ErrCode = FIFO_STATUS_OK;

	if(!pFifo->IsInitFifo)
		ErrCode = FIFO_STATUS_NOT_INIT;
	else if(pFifo->Size - pFifo->Count < Lenght)
		ErrCode = FIFO_STATUS_FREE_SIZE;

	return ErrCode;
}

/*!
 * \brief Checks if the employed number of bytes
 *
 * \param[IN] pFifo   	Pointer to the FIFO object
 * \parem[IN] Lenght	Data lenght
 * \retval 				Status of the operation
 */
static FifoStatus_t IsFifoEmployedSize(Fifo_t *pFifo, uint16_t Lenght)
{
	FifoStatus_t ErrCode = FIFO_STATUS_OK;

	if(!pFifo->IsInitFifo)
		ErrCode = FIFO_STATUS_NOT_INIT;
	else if(pFifo->Count < Lenght)
		ErrCode = FIFO_STATUS_EMPLOYED_SIZE;

	return ErrCode;
}

/*!
 * \brief Checks if the FIFO is empty
 *
 * \param[IN] pFifo   	Pointer to the FIFO object
 * \retval 				Status of the operation
 */
static FifoStatus_t IsFifoEmpty(Fifo_t *pFifo)
{
	FifoStatus_t ErrCode = FIFO_STATUS_OK;

	if(!pFifo->IsInitFifo)
		ErrCode = FIFO_STATUS_NOT_INIT;
	else if(!pFifo->Count)
		ErrCode = FIFO_STATUS_EMPTY;

	return ErrCode;
}

/*!
 * Checks if the FIFO is full
 *
 * \param[IN] pFifo   	Pointer to the FIFO object
 * \retval 				Status of the operation
 */
static FifoStatus_t IsFifoFull(Fifo_t *pFifo)
{
	FifoStatus_t ErrCode = FIFO_STATUS_OK;

	if(!pFifo->IsInitFifo)
		ErrCode = FIFO_STATUS_NOT_INIT;
	else if(pFifo->Count == pFifo->Size)
		ErrCode = FIFO_STATUS_FULL;

	return ErrCode;
}

/*!
 * Get next index the FIFO structure
 *
 * \param[IN] pFifo   	Pointer to the FIFO object
 * \param[IN] Index 	Index
 * \retval 				Next index
 */
static uint16_t FifoNextIndex(Fifo_t *pFifo, uint16_t Index)
{
    return (Index + 1) % pFifo->Size;
}

/*!
 * Initializes the FIFO structure
 *
 * \param[IN] pFifo   	Pointer to the FIFO object
 * \param[IN] pData 	Pointer buffer to be used as FIFO
 * \param[IN] Size   	Size of the buffer
 * \retval 				Status of the operation
 */
FifoStatus_t FifoInit(Fifo_t *pFifo, uint8_t *pData, uint16_t Size)
{
	FifoStatus_t ErrCode = FIFO_STATUS_OK;

	pFifo->Begin = 0;
	pFifo->End = 0;
	pFifo->Count = 0;
	pFifo->pData = pData;
	pFifo->Size = Size;
	pFifo->IsInitFifo = 1;

	return ErrCode;
}

/*!
 * Put symbol to the FIFO
 *
 * \param[IN] pFifo 	Pointer to the FIFO object
 * \param[IN] Ch 		Data to be pushed into the FIFO
 * \retval 				Status of the operation
 */
FifoStatus_t FifoPutChar(Fifo_t *pFifo, uint8_t Ch)
{
	FifoStatus_t ErrCode = FIFO_STATUS_OK;

	if((ErrCode = IsFifoFull(pFifo)) != FIFO_STATUS_OK)
		return ErrCode;

	FIFO_BEGIN_CRITICAL_SECTION();

	pFifo->pData[pFifo->End] = Ch;
	pFifo->End = FifoNextIndex(pFifo, pFifo->End);
	pFifo->Count++;

	FIFO_END_CRITICAL_SECTION();

	return ErrCode;
}

/*!
 * Get symbol from the FIFO
 *
 * \param[IN] pFifo 	Pointer to the FIFO object
 * \param[IN] pCh 		Data pointer
 * \retval 				Status of the operation
 */
FifoStatus_t FifoGetChar(Fifo_t *pFifo, uint8_t *pCh)
{
	FifoStatus_t ErrCode = FIFO_STATUS_OK;
	uint8_t uTemp;

	if((ErrCode = IsFifoEmpty(pFifo)) != FIFO_STATUS_OK)
		return ErrCode;

	FIFO_BEGIN_CRITICAL_SECTION();

	uTemp = pFifo->pData[pFifo->Begin];
	pFifo->Begin = FifoNextIndex(pFifo, pFifo->Begin);
	pFifo->Count--;
	*pCh = uTemp;

	FIFO_END_CRITICAL_SECTION();

	return ErrCode;
}

/*!
 * Put buf to the FIFO
 *
 * \param[IN] pFifo 	Pointer to the FIFO object
 * \param[IN] pData 	Data pointer
 * \param[IN] Size 		Data size
 * \retval 				Status of the operation
 */
FifoStatus_t FifoPutBuf(Fifo_t *pFifo, uint8_t *pData, uint16_t Size)
{
	FifoStatus_t ErrCode = FIFO_STATUS_OK;

	if((ErrCode = IsFifoFull(pFifo)) != FIFO_STATUS_OK ||
	   (ErrCode = IsFifoFreeSize(pFifo, Size)) != FIFO_STATUS_OK)
		return ErrCode;

	FIFO_BEGIN_CRITICAL_SECTION();

	memcpy(pFifo->pData + pFifo->Count, pData, Size);
	pFifo->Count += Size;
	pFifo->End = FifoNextIndex(pFifo, pFifo->End + Size - 1);

	FIFO_END_CRITICAL_SECTION();

	return ErrCode;
}

/*!
 * Get buf from the FIFO
 *
 * \param[IN] pFifo 	Pointer to the FIFO object
 * \param[IN] pData 	Data pointer
 * \param[IN] Size 		Data size
 * \retval 				Status of the operation
 */
FifoStatus_t FifoGetBuf(Fifo_t *pFifo, uint8_t *pData, uint16_t Size)
{
	FifoStatus_t ErrCode = FIFO_STATUS_OK;

	if((ErrCode = IsFifoEmpty(pFifo)) != FIFO_STATUS_OK ||
	   (ErrCode = IsFifoEmployedSize(pFifo, Size)) != FIFO_STATUS_OK)
		return ErrCode;

	FIFO_BEGIN_CRITICAL_SECTION();

	memcpy(pData, pFifo->pData, Size);
	pFifo->Count -= Size;
	pFifo->Begin = FifoNextIndex(pFifo, pFifo->Begin + Size - 1);

	FIFO_END_CRITICAL_SECTION();

	return ErrCode;
}

/*!
 * Flushes the FIFO
 *
 * \param[IN] pFifo   	Pointer to the FIFO object
 * \retval 				Status of the operation
 */
FifoStatus_t FifoFlush(Fifo_t *pFifo)
{
	FifoStatus_t ErrCode = FIFO_STATUS_OK;

	pFifo->Begin = 0;
	pFifo->End = 0;
	pFifo->Count = 0;
	memset(pFifo->pData, 0, pFifo->Size);

	return ErrCode;
}
