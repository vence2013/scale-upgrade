#include "em_chip.h"
#include "bsp_trace.h"
#include "driver.h"
#include "debug.h"

int main(void)
{
	/* Chip errata */
	CHIP_Init();
	/* If first word of user data page is non-zero, enable Energy Profiler trace */
	BSP_TraceProfilerSetup();

	Driver_setup();

	DBG_adc_single();

	/* Infinite loop */
	while (1) {
		DBG_blinky();
		//DBG_dac_sine();
	}
}
