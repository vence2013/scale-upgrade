#include "em_cmu.h"
#include "em_adc.h"
#include "debug.h"
#include "utils.h"
#include "driver.h"
#include <stdio.h>
#include <string.h>


static void adc_single_setup( void )
{
	ADC_Init_TypeDef       init       = ADC_INIT_DEFAULT;
	ADC_InitSingle_TypeDef singleInit = ADC_INITSINGLE_DEFAULT;

	/* Enable ADC clock */
	CMU_ClockEnable(cmuClock_ADC0, true);

	/* Init common settings for both single conversion and scan mode */
	init.timebase = ADC_TimebaseCalc(0);
	init.prescale = ADC_PrescaleCalc(7000000, 0);
	ADC_Init(ADC0, &init);

#if 0
	/* Init for single conversion, measure VDD/3 with 1.25V reference. */
	singleInit.input      = adcSingleInpVDDDiv3;
#elif 0
	/* ch2/PD2 */
	singleInit.reference  = adcRefVDD;
	singleInit.input      = adcSingleInpCh2;
#elif 1
	/* ch2/PD2, ch3/PD3 */
	singleInit.reference  = adcRef5VDIFF;
	singleInit.diff = true;
	singleInit.input      = adcSingleInputCh2Ch3;
#endif
	/* The datasheet specifies a minimum aquisition time when sampling VDD/3 */
	/* 32 cycles should be safe for all ADC clock frequencies */
	singleInit.acqTime = adcAcqTime32;
	ADC_InitSingle(ADC0, &singleInit);
}

void DBG_adc_single( void )
{
	u32 sampleValue, volt;
	u8 buf[64];

	adc_single_setup();

	while(1)
	{
		ADC_Start(ADC0, adcStartSingle);

		/* Wait while conversion is active */
		while (ADC0->STATUS & ADC_STATUS_SINGLEACT) ;
		/* Get ADC result */
		sampleValue = ADC_DataSingleGet(ADC0);
		/* Calculate supply voltage relative to 1.25V reference */
		//volt = (sampleValue * 1250 * 3) / 4096;
		//volt = (sampleValue * 3300) / 4096;
		volt = (sampleValue * 5000) / 4096;

		memset((char *)buf, 0, sizeof(buf));
		sprintf((char *)buf, "adc:%d, volt:%d.\r\n", sampleValue, volt);
		usart_put_data(buf, strlen((char *)buf));

		DBG_blinky();
	}
}
