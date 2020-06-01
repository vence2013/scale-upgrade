#ifndef __PINDEF_H__
#define __PINDEF_H__


/* Include Files ------------------------------------------------------------*/

#include "em_gpio.h"


/* Macro Definitions --------------------------------------------------------*/

/* LED */
#define LED_Port                                          (gpioPortE)
#define LED0_Pin                                                  (2)
#define LED1_Pin                                                  (3)

/* UART1 */
#define UART_Print_Port                                   (gpioPortD)
#define UART_Print_RX_Pin                                         (1)
#define UART_Print_TX_Pin                                         (0)


/* Structure Definitions ----------------------------------------------------*/

/* Exported Variables -------------------------------------------------------*/

/* Exported Functions -------------------------------------------------------*/


#endif
