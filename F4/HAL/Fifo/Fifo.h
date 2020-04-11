/*!
 * \file      Fifo.h
 *
 * \brief     FIFO buffer implementation
 *
 * \author    Anosov Anton
 */

#ifndef FIFO_H_
#define FIFO_H_
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <stdint.h>
#include <string.h>

#define FIFO_BEGIN_CRITICAL_SECTION()		__disable_irq()
#define FIFO_END_CRITICAL_SECTION()			__enable_irq()

/*!
 * FIFO structure
 */
typedef struct Fifo_s
{
	/*!
     * Start of the fifo buffer
     */
	uint16_t Begin;

	/*!
     * End of the fifo buffer
     */
    uint16_t End;

	/*!
     * Counter symbol fifo buffer
     */
    uint16_t Count;

	/*!
     * Data pointer
     */
    uint8_t *pData;

	/*!
     * Size fifo buffer
     */
    uint16_t Size;

    /*!
     * Is init fifo buffer
     */
    uint8_t IsInitFifo:1;

}Fifo_t;

/*!
 * FIFO status enum
 */
typedef enum FifoStatus_e
{
	/*!
     * No error occurred
     */
	FIFO_STATUS_OK = 0,

    /*!
     * Error fifo not init
     */
	FIFO_STATUS_NOT_INIT,

    /*!
     * Error fifo empty
     */
	FIFO_STATUS_EMPTY,

    /*!
     * Error fifo full
     */
	FIFO_STATUS_FULL,

    /*!
     * Error fifo free size
     */
	FIFO_STATUS_FREE_SIZE,

    /*!
     * Error fifo employed size
     */
	FIFO_STATUS_EMPLOYED_SIZE

}FifoStatus_t;

/*!
 * Initializes the FIFO structure
 *
 * \param[IN] pFifo   	Pointer to the FIFO object
 * \param[IN] pData 	Pointer buffer to be used as FIFO
 * \param[IN] Size   	Size of the buffer
 * \retval 				Status of the operation
 */
FifoStatus_t FifoInit(Fifo_t *pFifo, uint8_t *pData, uint16_t Size);

/*!
 * Put symbol to the FIFO
 *
 * \param[IN] pFifo 	Pointer to the FIFO object
 * \param[IN] Ch 		Data to be pushed into the FIFO
 * \retval 				Status of the operation
 */
FifoStatus_t FifoPutChar(Fifo_t *pFifo, uint8_t Ch);

/*!
 * Get symbol from the FIFO
 *
 * \param[IN] pFifo 	Pointer to the FIFO object
 * \param[IN] pCh 		Data pointer
 * \retval 				Status of the operation
 */
FifoStatus_t FifoGetChar(Fifo_t *pFifo, uint8_t *pCh);

/*!
 * Put buf to the FIFO
 *
 * \param[IN] pFifo 	Pointer to the FIFO object
 * \param[IN] pData 	Data pointer
 * \param[IN] Size 		Data size
 * \retval 				Status of the operation
 */
FifoStatus_t FifoPutBuf(Fifo_t *pFifo, uint8_t *pData, uint16_t Size);

/*!
 * Get buf from the FIFO
 *
 * \param[IN] pFifo 	Pointer to the FIFO object
 * \param[IN] pData 	Data pointer
 * \param[IN] Size 		Data size
 * \retval 				Status of the operation
 */
FifoStatus_t FifoGetBuf(Fifo_t *pFifo, uint8_t *pData, uint16_t Size);

/*!
 * Flushes the FIFO
 *
 * \param[IN] pFifo   	Pointer to the FIFO object
 * \retval 				Status of the operation
 */
FifoStatus_t FifoFlush(Fifo_t *pFifo);

#ifdef __cplusplus
}
#endif
#endif /* FIFO_H_ */
