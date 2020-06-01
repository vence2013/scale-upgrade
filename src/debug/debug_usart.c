#include "debug.h"
#include "utils.h"
#include "driver.h"
#include <stdio.h>
#include <string.h>


/**************************************************************************//**
 * @brief
 *
 * UART1(PB9/PB10) send & receive
 *
 * Notice that can use with DBG_blinky()
 *****************************************************************************/
void DBG_usart( void )
{
	static u8 tCnt = 0;

#if 0 /* send 1 byte */
	usart_put_char(tCnt);
#elif 0 /* receive & send byte */
	s32 ret;
	u8 ch;

	ret = usart_get_char(&ch);
	if (!ret)
	{
		usart_put_char(ch);
	}
#elif 1 /* receive & send data */
	s32 ret;
	u8 recv[BUFFERSIZE], send[BUFFERSIZE];

	memset(recv, 0, BUFFERSIZE);
	memset(send, 0, BUFFERSIZE);
	ret = usart_get_data(recv, 0);
	if (ret > 0)
	{
		sprintf((char *)send, "No %d, recv %d byte: %s.\r\n", tCnt, ret, recv);
	}
	else
	{
		sprintf((char *)send, "No %d.\r\n", tCnt);
	}
	usart_put_data(send, strlen((char *)send));
#endif

	tCnt++;
}
