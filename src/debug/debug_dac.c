#include "em_cmu.h"
#include "em_dac.h"
#include "em_prs.h"
#include "em_timer.h"
#include "debug.h"
#include "utils.h"
#include "driver.h"


static void dac_voltage_setup( void )
{
	DAC_Init_TypeDef stdacinit = {
		.refresh = dacRefresh8, /* 配置刷新率 */
		.reference = dacRef2V5, /* 参考电压配置 */
		.outMode = dacOutputPin, /* 配置 DAC 输出 */
		.convMode = dacConvModeContinuous, /* 配置 DAC 转换模式 */
		.prescale = DAC_PrescaleCalc(1000000, 0), /* 配置 DAC 模块时钟 */
		.lpEnable = false, /* 禁能低功耗模式 */
		.ch0ResetPre = false,
		.outEnablePRS = false,
		.sineEnable = false, /* 禁能正弦波输出 */
		.diff = false, /* 禁能差分输出 */
	};
	DAC_InitChannel_TypeDef stdacinitChannel = {
		.enable = false,
		.prsEnable = false,
		.refreshEnable = false,
		.prsSel = dacPRSSELCh0,
	};

	CMU_ClockEnable(cmuClock_DAC0, true);

	DAC_Init(DAC0, &stdacinit);
	DAC_InitChannel(DAC0, &stdacinitChannel, 0);
}

void DBG_dac_voltage( void )
{
	dac_voltage_setup();

	DAC_Enable(DAC0, 0, true);
	DAC0->CH0DATA = (int)((1.5 * 4095) / 2.5); /* 在 PB11 管脚输出 1.5V */
}

/* dac + sine */
static void dac_sine_setup( void )
{
	/* Use default timer */
	TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
	DAC_Init_TypeDef        init        = DAC_INIT_DEFAULT;
	DAC_InitChannel_TypeDef initChannel = DAC_INITCHANNEL_DEFAULT;

	CMU_ClockEnable(cmuClock_TIMER0, true);
	CMU_ClockEnable(cmuClock_PRS, true);

	TIMER_Init(TIMER0, &timerInit);

	/* PRS setup */
	/* Select TIMER0 as source and TIMER0OF (Timer0 overflow) as signal */
	PRS_SourceSignalSet(0, PRS_CH_CTRL_SOURCESEL_TIMER0, PRS_CH_CTRL_SIGSEL_TIMER0OF, prsEdgeOff);


	/* Enable the DAC clock */
	CMU_ClockEnable(cmuClock_DAC0, true);

	/* Calculate the DAC clock prescaler value that will result in a DAC clock
	* close to 1 MHz. Second parameter is zero, if the HFPERCLK value is 0, the
	* function will check what the HFPERCLK actually is. */
	init.prescale = DAC_PrescaleCalc(1000000, 0);
	/* Disable PRS control of output driver. PRS is instead used to trigger new samples */
	init.outEnablePRS = false;
	/* Enable Sine Generator Mode */
	init.sineEnable = true;
	/* Initialize the DAC and DAC channel. */
	DAC_Init(DAC0, &init);

	initChannel.prsEnable = true;
	initChannel.prsSel    = dacPRSSELCh0;
	DAC_InitChannel(DAC0, &initChannel, 0);

	/* Enable DAC channel 0, located on pin PB11 */
	DAC_Enable(DAC0, 0, true);
}

/**************************************************************************//**
 * @brief  Calculate and set new timer top value
 * Calculates and sets new timer top value based on current hfperclk
 *****************************************************************************/
void SineFrequencySet(uint32_t freq)
{
  uint32_t TimerTopValue;
  /* Get peripheral clock frequency */
  uint32_t hfperFreq = CMU_ClockFreqGet(cmuClock_HFPER);

  /* Calculate new timer top value */
  TimerTopValue = (hfperFreq >> 4) / freq;

  if (TimerTopValue > 0xffff)
  {
    TimerTopValue = 0xffff;
  }

  TIMER_TopBufSet(TIMER0, TimerTopValue);
}

void DBG_dac_sine( void )
{
	dac_sine_setup();

	/* Set the desired frequency in Hz, preferably 20 Hz - 15 kHz */
	SineFrequencySet(15000);
}
