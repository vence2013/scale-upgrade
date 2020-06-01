#ifndef __USART_H__
#define __USART_H__


/* Include Files ------------------------------------------------------------*/

#include "typedef.h"


/* Macro Definitions --------------------------------------------------------*/

/* Declare a circular buffer structure to use for Rx and Tx queues */
#define BUFFERSIZE                                                256


/* Structure Definitions ----------------------------------------------------*/

/* Exported Variables -------------------------------------------------------*/

/* Exported Functions -------------------------------------------------------*/

void usart_setup( void );
s32 usart_get_char(u8* ch);
s32 usart_put_char(u8 ch);
s32 usart_get_data(u8 * dataPtr, u32 dataLen);
s32 usart_put_data(u8 * dataPtr, u32 dataLen);

#endif
