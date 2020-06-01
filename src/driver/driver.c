#include "driver.h"
#include "pindef.h"


/******************************************************************************
 * @brief  Driver_led function
 *
 *****************************************************************************/
void Driver_led(u8 mask, u8 ctrl)
{
	if (mask&LED0)
	{
		if (ctrl&LED0)
		{
			GPIO_PinOutSet(LED_Port, LED0_Pin);
		}
		else
		{
			GPIO_PinOutClear(LED_Port, LED0_Pin);
		}
	}
	if (mask&LED1)
	{
		if (ctrl&LED1)
		{
			GPIO_PinOutSet(LED_Port, LED1_Pin);
		}
		else
		{
			GPIO_PinOutClear(LED_Port, LED1_Pin);
		}
	}
}

/******************************************************************************
 * @brief  Driver_setup function
 *
 *****************************************************************************/
void Driver_setup( void )
{
	usart_setup();
}
