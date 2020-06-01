#include "em_cmu.h"
#include "em_dac.h"
#include "debug.h"
#include "utils.h"
#include "driver.h"


static void dac_init( void )
{
	DAC_Init_TypeDef stdacinit = {
		.refresh = dacRefresh8, /* ����ˢ���� */
		.reference = dacRef2V5, /* �ο���ѹ���� */
		.outMode = dacOutputPin, /* ���� DAC ��� */
		.convMode = dacConvModeContinuous, /* ���� DAC ת��ģʽ */
		.prescale = DAC_PrescaleCalc(1000000, 0), /* ���� DAC ģ��ʱ�� */
		.lpEnable = false, /* ���ܵ͹���ģʽ */
		.ch0ResetPre = false,
		.outEnablePRS = false,
		.sineEnable = false, /* �������Ҳ���� */
		.diff = false, /* ���ܲ����� */
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
	DAC0->CH0DATA = (int)((1.5 * 4095) / 2.5); /* �� PB11 �ܽ���� 1.5V */
}

/* dac + sine */
/* dac + dma + timer */
