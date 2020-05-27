#include "debug.h"
#include "utils.h"
#include "driver.h"

/* Check data size.
 *   use for typedef in typedef.h
 */
void DBG_data_size( void )
{
	char sizes[4] = {0};

	sizes[0] = sizeof(char);
	sizes[1] = sizeof(short);
	sizes[2] = sizeof(int);
	sizes[3] = sizes[0] + 1;
}

/* LED blinky.
 *   test items: 1. LED control; 2. software delay
 */
void DBG_blinky( void )
{
	static u8 tCnt = 0;

	Driver_led(LED_ALL, (tCnt%2) ? LED_ALL : 0);
	delay_ms(1000);
	tCnt++;
}
