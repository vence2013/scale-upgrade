#include "em_cmu.h"
#include "em_dac.h"
#include "em_adc.h"
#include "em_opamp.h"
#include "debug.h"
#include "utils.h"
#include "driver.h"
#include <stdio.h>
#include <string.h>


static void opamp_non_inverting_setup( void )
{
	OPAMP_Init_TypeDef initOpa = OPA_INIT_NON_INVERTING;

	// Enable DAC0 clock
	CMU_ClockEnable(cmuClock_DAC0, true);

	/* Configure OPA0
	 * P0/PC4, N0/PC5, OUTPUT0/PB11
	 */
	initOpa.outMode = opaOutModeAll;
	initOpa.resSel  = opaResSelR2eqR1;
	initOpa.resInMux = opaResInMuxVss;
	initOpa.outPen   = DAC_OPA0MUX_OUTPEN_OUT4;

	// Enable OPA
	OPAMP_Enable(DAC0, OPA0, &initOpa);
}

static void opamp_adc_setup( void )
{
	ADC_Init_TypeDef initAdc = 	ADC_INIT_DEFAULT;
	ADC_InitSingle_TypeDef initSingleAdc = ADC_INITSINGLE_DEFAULT;

	CMU_ClockEnable(cmuClock_ADC0, true);

	/* Init common settings for both single conversion and scan mode */
	initAdc.timebase = ADC_TimebaseCalc(0);
	initAdc.prescale = ADC_PrescaleCalc(7000000, 0);
	ADC_Init(ADC0, &initAdc);

	initSingleAdc.reference = adcRefVDD;
	ADC_InitSingle(ADC0, &initSingleAdc);
}

/* not working */
void DBG_opamp_adc( void )
{
	u16 sampleValue;
	u8 buf[64];

	opamp_non_inverting_setup();
	opamp_adc_setup();

	while(1)
	{
		ADC_Start(ADC0, adcStartSingle);
		/* Wait while conversion is active */
		while (ADC0->STATUS & ADC_STATUS_SINGLEACT)
		;

		/* Get ADC result */
		sampleValue = ADC_DataSingleGet(ADC0);
		memset(buf, 0, sizeof(buf));
		sprintf((char *)buf, "opamp & adc:%d.\r\n", sampleValue);
		usart_put_data(buf, strlen((char *)buf));

		DBG_blinky();
	}
}
