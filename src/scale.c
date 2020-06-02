#include "segmentlcd.h"
#include "driver.h"
#include <stdio.h>
#include <string.h>


/******************************************************************************
 * @brief
 *****************************************************************************/
void scale_application( void )
{
	static u32 tickPrevious = 0;
	s32 ret, volt;
	u8 buf[64];

	if (tickPrevious != Driver_tick)
	{
		ret = Driver_sensor(&volt);

		Driver_led_toggle( LED0 );

		/* LCD display */
		memset(buf, 0, sizeof(buf));
		sprintf((char *)buf, "% d", volt);
		SegmentLCD_Write((const char *)buf);
		/* USART Printf */
		memset(buf, 0, sizeof(buf));
		sprintf((char *)buf, "ret:%d, sensor:%d, tick:%ld.\r\n", ret, volt, Driver_tick);
		usart_put_data(buf, strlen((char *)buf));
	}
	tickPrevious = Driver_tick;
}
