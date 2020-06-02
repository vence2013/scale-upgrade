#include "em_cmu.h"
#include "em_dac.h"
#include "em_adc.h"
#include "em_opamp.h"
#include "debug.h"
#include "utils.h"
#include "driver.h"
#include <stdio.h>
#include <string.h>


/* not diff */
static void adc_ch0_setup( void )
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

static u32 adc_ch0_single( void )
{
	ADC_Start(ADC0, adcStartSingle);
	/* Wait while conversion is active */
	while (ADC0->STATUS & ADC_STATUS_SINGLEACT)	;

	/* Get ADC result */
	return ADC_DataSingleGet(ADC0);
}

/* 将模拟信号接入PC4后，在PB11上电压跟随 */
static void opamp_ug_setup( void )
{
	OPAMP_Init_TypeDef initOpa = OPA_INIT_UNITY_GAIN;

	// Enable DAC0 clock
	CMU_ClockEnable(cmuClock_DAC0, true);

	/* Configure OPA0: P0/PC4, OUTPUT0/PB11 */
	initOpa.outMode = opaOutModeAll;
	initOpa.outPen  = DAC_OPA0MUX_OUTPEN_OUT4;
	OPAMP_Enable(DAC0, OPA0, &initOpa);
}

static void opamp_non_inverting( void )
{
	OPAMP_Init_TypeDef initOpa = OPA_INIT_NON_INVERTING;

	// Enable DAC0 clock
	CMU_ClockEnable(cmuClock_DAC0, true);

	/* Configure OPA0: P0/PC4, OUTPUT0/PB11
	 * VIN = 105mV, VOUT = 105*(1+15)= 1680mV, adc: 1690
	 */
	initOpa.resSel  = opaResSelR2eq15R1;
	initOpa.resInMux= opaResInMuxVss;
	initOpa.outMode = opaOutModeAll;
	initOpa.outPen  = DAC_OPA0MUX_OUTPEN_OUT4;
	OPAMP_Enable(DAC0, OPA0, &initOpa);
}

static void opamp_cascaded_non_inverting( void )
{
	OPAMP_Init_TypeDef
		init0 = OPA_INIT_CASCADED_NON_INVERTING_OPA0,
		init1 = OPA_INIT_CASCADED_NON_INVERTING_OPA1,
		init2 = OPA_INIT_CASCADED_NON_INVERTING_OPA2;

	// Enable DAC0 clock
	CMU_ClockEnable(cmuClock_DAC0, true);

	/* Configure OPA0: P0/PC4, VOUT = VIN * (1 + 2) */
	init0.resSel  = opaResSelR2eq2R1;
	init0.resInMux= opaResInMuxVss;
	OPAMP_Enable(DAC0, OPA0, &init0);

	init1.resSel  = opaResSelR2eq2R1;
	init1.resInMux= opaResInMuxVss;
	OPAMP_Enable(DAC0, OPA1, &init1);

	/* OUT1/PD0 */
	init2.resSel  = opaResSelR2eq2R1;
	init2.resInMux= opaResInMuxVss;
	init2.outPen  = DAC_OPA0MUX_OUTPEN_OUT1;
	OPAMP_Enable(DAC0, OPA2, &init2);
}

static void adc_ch0ch1_setup( void )
{
	ADC_Init_TypeDef initAdc = 	ADC_INIT_DEFAULT;
	ADC_InitSingle_TypeDef initSingleAdc = ADC_INITSINGLE_DEFAULT;

	CMU_ClockEnable(cmuClock_ADC0, true);

	/* Init common settings for both single conversion and scan mode */
	initAdc.timebase = ADC_TimebaseCalc(0);
	initAdc.prescale = ADC_PrescaleCalc(7000000, 0);
	ADC_Init(ADC0, &initAdc);

	initSingleAdc.input = adcSingleInpCh0Ch1;
	initSingleAdc.diff  = true;
	initSingleAdc.reference = adcRefVDD;
	ADC_InitSingle(ADC0, &initSingleAdc);
}

static void opamp_two_differential( void )
{
	OPAMP_Init_TypeDef
		init1 = OPA_INIT_DIFF_DRIVER_OPA0,
		init2 = OPA_INIT_DIFF_DRIVER_OPA1;

	// Enable DAC0 clock
	CMU_ClockEnable(cmuClock_DAC0, true);

	/* OPA1: P1/PD6, OUTPUT: ADC CH1/PD1 */
	init1.outMode = opaOutModeAll;
	init1.outPen  = DAC_OPA0MUX_OUTPEN_OUT4; /* output to adc ch1 */
	OPAMP_Enable(DAC0, OPA1, &init1);

	/* OPA2: P2/PD4, OUTPUT: ADC CH0/PD0 */
	init2.resSel  = opaResSelR2eq15R1;
	init2.outPen  = DAC_OPA0MUX_OUTPEN_OUT1; /* output to adc ch0*/
	OPAMP_Enable(DAC0, OPA2, &init2);

	/* Differential Output: P1/PD0 - OUT1/PD1
	 * V1=0mV, V2=98mV;
	 *   R2/R1=3, Vdiff = 380mV
	 *   R2/R1=7, Vdiff = 732mV
	 *   R2/R1=15,Vdiff = 1376mV
	 */
}

void DBG_opamp_adc( void )
{
	u32 adv, volt;
	u8 buf[64];

#if 0 /* not differential */
	adc_ch0_setup();
	opamp_cascaded_non_inverting();
#else
	adc_ch0ch1_setup();
	opamp_two_differential();
#endif
	while(1)
	{
		adv = adc_ch0_single();
		volt = (adv * 3300) / 4096;
		memset(buf, 0, sizeof(buf));
		sprintf((char *)buf, "adv:%d, volt:%d.\r\n", adv, volt);
		usart_put_data(buf, strlen((char *)buf));

		DBG_blinky();
	}
}
