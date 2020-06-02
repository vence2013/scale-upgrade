#ifndef __DRIVER_H__
#define __DRIVER_H__


/* Include Files ------------------------------------------------------------*/

#include "typedef.h"
#include "usart.h"


/* Macro Definitions --------------------------------------------------------*/

/* Structure Definitions ----------------------------------------------------*/

enum { LED0 = 0X01, LED1 = 0X02, LED_ALL = 0X03 };


/* Exported Variables -------------------------------------------------------*/

extern u32 Driver_tick;


/* Exported Functions -------------------------------------------------------*/

void Driver_led(u8 mask, u8 ctrl);
void Driver_led_toggle(u8 sel);
s32 Driver_sensor(s32* volt);
void Driver_setup( void );

#endif
