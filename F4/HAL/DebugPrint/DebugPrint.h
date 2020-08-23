/*!
 * @file      DebugPrint.h
 *
 * @brief     Debug print module
 *
 * @author    Anosov Anton
 */

#ifndef DEBUGPRINT_H_
#define DEBUGPRINT_H_
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

extern UART_HandleTypeDef 	 huart1;
#define DEBUG_UART		 	 huart1

/*!
 * @brief Function that outputs a string from the buffer
 *
 * @param pString			Pointer to the string
 */
void DebugPrint(const char *pString);

/*!
 * @brief Function that outputs a string with data from the buffer
 *
 * @param pFormat			Pointer to the string with data
 */
void DebugPrintf(const char *pFormat, ...);

/*
 * Use TOSTRING() when printing #defines as strings.
 * (STRINGIFY is simply a helper in this two step procedure)
 */
#define STRINGIFY(s) 		 #s
#define TOSTRING(s) 		 STRINGIFY(s)

#if !defined(NO_DEBUGPRINT) && defined(DEBUGPRINT)
	#define DPRINTF(PFORMAT, ...)   ( DebugPrintf( (PFORMAT), ## __VA_ARGS__ ) )
	#define DPRINT(PSTRING)         ( DebugPrint( (PSTRING) ) )
#else
	#define DPRINTF(PFORMAT, ...) 	do {} while(0)
	#define DPRINT(PSTRING)       	do {} while(0)
#endif

#ifdef __cplusplus
}
#endif
#endif /* DEBUGPRINT_H_ */
