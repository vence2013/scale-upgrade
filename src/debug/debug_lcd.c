#include "em_cmu.h"
#include "em_lcd.h"
#include "segmentlcd.h"
#include "debug.h"
#include "utils.h"
#include "driver.h"
#include <stdio.h>
#include <string.h>


static void lcd_setup( void )
{
	// Initialize LCD controller with current source mode.

	// LCD Controller Prescaler (divide LFACLK / 8)
	// LFACLK_LCDpre = 32768/8 => 4096 Hz
	// Set FRDIV = 4, means LFACLKlcd = 4096/(1+4) => 819.2 Hz
	// With Octaplex multiplexing, frame Rate = 819.2/16 => 51.2 Hz
	SegmentLCD_Init(false);

	// Write something to text field on LCD display.
	//SegmentLCD_Write("HELLO");
	SegmentLCD_Number(1234);
	SegmentLCD_Write("1234567890");
}

void DBG_lcd( void )
{
	lcd_setup();

	while(1)
	{
		;
	}
}
