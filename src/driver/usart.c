#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "usart.h"
#include "pindef.h"


volatile struct circularBuffer
{
  u8  data[BUFFERSIZE];  /* data buffer */
  u32 rdI;               /* read index */
  u32 wrI;               /* write index */
  u32 pendingBytes;      /* count of how many bytes are not yet handled */
  u8  overflow;          /* buffer overflow indicator */
} rxBuf, txBuf = { {0}, 0, 0, 0, 0 };

static USART_TypeDef* uart   = USART1;


/**************************************************************************//**
 * @brief USART1 RX IRQ Handler
 *
 * Set up the interrupt prior to use
 *
 * Note that this function handles overflows in a very simple way.
 *
 *****************************************************************************/
void USART1_RX_IRQHandler(void)
{
	/* Check for RX data valid interrupt */
	if (uart->STATUS & UART_STATUS_RXDATAV)
	{
		/* Copy data into RX Buffer */
		uint8_t rxData = USART_Rx(uart);

		rxBuf.data[rxBuf.wrI] = rxData;
		rxBuf.wrI             = (rxBuf.wrI + 1) % BUFFERSIZE;
		rxBuf.pendingBytes++;

		/* Flag Rx overflow */
		if (rxBuf.pendingBytes > BUFFERSIZE)
		{
			rxBuf.overflow = 1;
		}

		/* Clear RXDATAV interrupt */
		USART_IntClear(uart, UART_IF_RXDATAV);
	}
}

/**************************************************************************//**
 * @brief USART1 TX IRQ Handler
 *
 * Set up the interrupt prior to use
 *
 *****************************************************************************/
void USART1_TX_IRQHandler(void)
{
	/* Clear interrupt flags by reading them. */
	USART_IntGet(uart);

	/* Check TX buffer level status */
	if (uart->STATUS & UART_STATUS_TXBL)
	{
		if (txBuf.pendingBytes > 0)
		{
			/* Transmit pending character */
			USART_Tx(uart, txBuf.data[txBuf.rdI]);
			txBuf.rdI = (txBuf.rdI + 1) % BUFFERSIZE;
			txBuf.pendingBytes--;
		}

		/* Disable Tx interrupt if no more bytes in queue */
		if (txBuf.pendingBytes == 0)
		{
			USART_IntDisable(uart, UART_IF_TXBL);
		}
	}
}

/******************************************************************************
* @brief
*
******************************************************************************/
void usart_setup( void )
{
	USART_InitAsync_TypeDef uartInit = USART_INITASYNC_DEFAULT;

	/* Enable clock for GPIO module (required for pin configuration) */
	CMU_ClockEnable(cmuClock_GPIO, true);
	/* Configure GPIO pins */
	GPIO_PinModeSet(UART_Print_Port, UART_Print_TX_Pin, gpioModePushPull, 1);
	GPIO_PinModeSet(UART_Print_Port, UART_Print_RX_Pin, gpioModeInput, 0);

	/* Enable clock for HF peripherals */
	CMU_ClockEnable(cmuClock_HFPER, true);
	/* Enable clock for USART module */
	CMU_ClockEnable(cmuClock_USART1, true);

	/* Prepare struct for initializing UART in asynchronous mode*/
	uartInit.enable       = usartDisable;   /* Don't enable UART upon intialization */
	uartInit.refFreq      = 0;              /* Provide information on reference frequency. When set to 0, the reference frequency is */
	uartInit.baudrate     = 115200;         /* Baud rate */
	uartInit.oversampling = usartOVS16;     /* Oversampling. Range is 4x, 6x, 8x or 16x */
	uartInit.databits     = usartDatabits8; /* Number of data bits. Range is 4 to 10 */
	uartInit.parity       = usartNoParity;  /* Parity mode */
	uartInit.stopbits     = usartStopbits1; /* Number of stop bits. Range is 0 to 2 */
	uartInit.mvdis        = false;          /* Disable majority voting */
	uartInit.prsRxEnable  = false;          /* Enable USART Rx via Peripheral Reflex System */
	uartInit.prsRxCh      = usartPrsRxCh0;  /* Select PRS channel if enabled */

	/* Initialize USART with uartInit struct */
	USART_InitAsync(uart, &uartInit);

	/* Enable I/O pins at USART1 location #2 */
	uart->ROUTE = UART_ROUTE_RXPEN | UART_ROUTE_TXPEN | UART_ROUTE_LOCATION_LOC1;

	/* Prepare UART Rx and Tx interrupts */
	USART_IntClear(uart, _UART_IF_MASK);
	USART_IntEnable(uart, UART_IF_RXDATAV);
	NVIC_ClearPendingIRQ(USART1_RX_IRQn);
	NVIC_ClearPendingIRQ(USART1_TX_IRQn);
	NVIC_EnableIRQ(USART1_RX_IRQn);
	NVIC_EnableIRQ(USART1_TX_IRQn);

	/* Enable UART */
	USART_Enable(uart, usartEnable);
}

/******************************************************************************
 * @brief  usart_get_char function
 *
 *  Note that if there are no pending characters in the receive buffer, this
 *  function will hang until a character is received.
 *
 *****************************************************************************/
s32 usart_get_char(u8* ch)
{
	/* Check if there is a byte that is ready to be fetched. If no byte is ready, wait for incoming data */
	if (rxBuf.pendingBytes < 1)
	{
		return -1;
	}

	/* Copy data from buffer */
	*ch       = rxBuf.data[rxBuf.rdI];
	rxBuf.rdI = (rxBuf.rdI + 1) % BUFFERSIZE;

	/* Decrement pending byte counter */
	rxBuf.pendingBytes--;

	return 0;
}

/******************************************************************************
 * @brief  usart_put_char function
 *
 *****************************************************************************/
s32 usart_put_char(u8 ch)
{
	/* Check if Tx queue has room for new data */
	if ((txBuf.pendingBytes + 1) > BUFFERSIZE)
	{
		return -1;
	}

	/* Copy ch into txBuffer */
	txBuf.data[txBuf.wrI] = ch;
	txBuf.wrI             = (txBuf.wrI + 1) % BUFFERSIZE;

	/* Increment pending byte counter */
	txBuf.pendingBytes++;

	/* Enable interrupt on USART TX Buffer*/
	USART_IntEnable(uart, UART_IF_TXBL);

	return 0;
}

/******************************************************************************
 * @brief  uartGetData function
 *
 * Note that if dataLen is 0, return data in buffer
 *****************************************************************************/
s32 usart_get_data(u8 * dataPtr, u32 dataLen)
{
	u32 i = 0;

	if ((dataLen == 0) || (rxBuf.pendingBytes < dataLen))
	{
		dataLen = rxBuf.pendingBytes;
	}

	/* Copy data from Rx buffer to dataPtr */
	while (i < dataLen)
	{
		*(dataPtr + i) = rxBuf.data[rxBuf.rdI];
		rxBuf.rdI      = (rxBuf.rdI + 1) % BUFFERSIZE;
		i++;
	}

	/* Decrement pending byte counter */
	rxBuf.pendingBytes -= dataLen;

	return i;
}

/******************************************************************************
 * @brief  usart_put_data function
 *
 *****************************************************************************/
s32 usart_put_data(u8 * dataPtr, u32 dataLen)
{
	u32 i = 0;

	/* Check if buffer is large enough for data
	 * Check if buffer has room for new data
	 */
	if ((dataLen > BUFFERSIZE) || ((txBuf.pendingBytes + dataLen) > BUFFERSIZE))
	{
		/* Buffer can never fit the requested amount of data */
		return -1;
	}

	/* Fill dataPtr[0:dataLen-1] into txBuffer */
	while (i < dataLen)
	{
		txBuf.data[txBuf.wrI] = *(dataPtr + i);
		txBuf.wrI             = (txBuf.wrI + 1) % BUFFERSIZE;
		i++;
	}

	/* Increment pending byte counter */
	txBuf.pendingBytes += dataLen;

	/* Enable interrupt on USART TX Buffer*/
	USART_IntEnable(uart, UART_IF_TXBL);

	return 0;
}
