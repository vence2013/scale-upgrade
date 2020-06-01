#include "em_cmu.h"
#include "em_dac.h"
#include "debug.h"
#include "utils.h"
#include "driver.h"


static void dac_init( void )
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

void DBG_dac( void )
{
	dac_init();

	DAC_Enable(DAC0, 0, true);
	DAC0->CH0DATA = (int)((1.5 * 4095) / 2.5); /* 在 PB11 管脚输出 1.5V */
}

/* dac + sine */
/* dac + dma + timer */
