#include "em_cmu.h"
#include "em_adc.h"
#include "em_dac.h"
#include "em_opamp.h"
#include "segmentlcd.h"
#include "em_timer.h"
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

void Driver_led_toggle(u8 sel)
{
	u8 pin;

	switch (sel)
	{
		case LED0:
			GPIO_PinOutToggle(LED_Port, LED0_Pin);
			break;
		case LED1:
			GPIO_PinOutToggle(LED_Port, LED1_Pin);
			break;
	}
}

/******************************************************************************
 * @brief Driver_sensor_setup
 *   include OPAMP, ADC.
 *
 * Differential Input:  OPA1 P1/PD6, OPA2 P2/PD4
 *****************************************************************************/
static void sensor_setup( void )
{
	ADC_Init_TypeDef initAdc = 	ADC_INIT_DEFAULT;
	ADC_InitSingle_TypeDef initSingleAdc = ADC_INITSINGLE_DEFAULT;
	OPAMP_Init_TypeDef init1 = OPA_INIT_DIFF_DRIVER_OPA0;
	OPAMP_Init_TypeDef init2 = OPA_INIT_DIFF_DRIVER_OPA1;

	/* ADC setup */
	CMU_ClockEnable(cmuClock_ADC0, true);

	initAdc.timebase = ADC_TimebaseCalc(0);
	initAdc.prescale = ADC_PrescaleCalc(7000000, 0);
	ADC_Init(ADC0, &initAdc);

	initSingleAdc.input = adcSingleInpCh0Ch1;
	initSingleAdc.diff  = true;
	initSingleAdc.reference = adcRefVDD;
	ADC_InitSingle(ADC0, &initSingleAdc);

	/* OPAMP setup */
	CMU_ClockEnable(cmuClock_DAC0, true);

	init1.outMode = opaOutModeAll;
	init1.outPen  = DAC_OPA0MUX_OUTPEN_OUT4; /* output to adc ch1 */
	OPAMP_Enable(DAC0, OPA1, &init1);

	init2.resSel  = opaResSelR2eq7R1;
	init2.outPen  = DAC_OPA0MUX_OUTPEN_OUT1; /* output to adc ch0*/
	OPAMP_Enable(DAC0, OPA2, &init2);
}


/******************************************************************************
 * @brief Driver_sensor
 *   get the differentional voltage of sensor
 *****************************************************************************/
s32 Driver_sensor(s32* volt)
{
	ADC_Start(ADC0, adcStartSingle);
	/* Wait while conversion is active */
	while (ADC0->STATUS & ADC_STATUS_SINGLEACT)	;

	/* Get ADC result */
	*volt = ADC_DataSingleGet(ADC0);
	return 0;
}


#define TIMER0_FREQ                                             (500)

u32 Driver_tick = 0;

/**************************************************************************//**
 * @brief TIMER0_IRQHandler
 * Interrupt Service Routine TIMER0 Interrupt Line
 *****************************************************************************/
void TIMER0_IRQHandler(void)
{
	static u16 cnt = 0;

	/* Clear flag for TIMER0 overflow interrupt */
	TIMER_IntClear(TIMER0, TIMER_IF_OF);

	if (++cnt > TIMER0_FREQ) /* 1s */
	{
		cnt = 0;
		Driver_tick++;
	}
}

/******************************************************************************
 * @brief
 *   Use as tick
 *****************************************************************************/
static void timer0_setup( void )
{
	/* Select TIMER0 parameters */
	TIMER_Init_TypeDef timerInit = 	TIMER_INIT_DEFAULT;

	/* Enable clock for TIMER0 module */
	CMU_ClockEnable(cmuClock_TIMER0, true);

	/* Enable overflow interrupt */
	TIMER_IntEnable(TIMER0, TIMER_IF_OF);
	/* Enable TIMER0 interrupt vector in NVIC */
	NVIC_EnableIRQ(TIMER0_IRQn);

	/* Set TIMER Top value
	 * 14MHz / 500 = 2ms
	 */
	TIMER_TopSet(TIMER0, CMU_ClockFreqGet(cmuClock_HFPER)/TIMER0_FREQ);

	/* Configure TIMER */
	TIMER_Init(TIMER0, &timerInit);
}

/******************************************************************************
 * @brief  Driver_setup function
 *
 *****************************************************************************/
void Driver_setup( void )
{
	usart_setup();
	timer0_setup();

	sensor_setup();

	SegmentLCD_Init(false);
}
