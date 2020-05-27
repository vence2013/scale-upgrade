#include "em_chip.h"
#include "bsp_trace.h"
#include "debug.h"

int main(void)
{
	/* Chip errata */
	CHIP_Init();
	/* If first word of user data page is non-zero, enable Energy Profiler trace */
	BSP_TraceProfilerSetup();

	/* Infinite loop */
	while (1) {
		DBG_blinky();
	}
}
