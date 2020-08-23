/*!
 * @file      DebugPrint.c
 *
 * @brief     Debug print module
 *
 * @author    Anosov Anton
 */

#include "DebugPrint.h"

// Timeout for send UART
#define DEBUG_PRINT_TIMEOUT			1000

// Max size send buffer
#define DEBUG_PRINT_BUFFER_SIZE		1024

/*!
 * @brief Function that outputs a string from the buffer
 *
 * @param pString			Pointer to the string
 */
void DebugPrint(const char *pString)
{
	HAL_UART_Transmit(&DEBUG_UART, (uint8_t*)pString, strlen(pString), DEBUG_PRINT_TIMEOUT);
}

/*!
 * @brief Function that outputs a string with data from the buffer
 *
 * @param pFormat			Pointer to the string with data
 */
void DebugPrintf(const char *pFormat, ...)
{
    static char Buff[DEBUG_PRINT_BUFFER_SIZE];
    va_list Args;

    va_start(Args, pFormat);
    const uint16_t Length = vsnprintf(Buff, DEBUG_PRINT_BUFFER_SIZE, pFormat, Args);
    va_end(Args);

    HAL_UART_Transmit(&DEBUG_UART, (uint8_t*)(Buff), Length, DEBUG_PRINT_TIMEOUT);
}
