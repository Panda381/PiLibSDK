
// ****************************************************************************
//
//                                  UART0
//
// ****************************************************************************

#include "../../includes.h"	// includes

// set Baud rate
// Baud rate must no be changed:
// - when the UART is enabled
// - when completing a transmission or a reception
// - when it has been programmed to become disabled.
void UART0_SetBad(u32 baud)
{
	u32 div = (UARTClock*4 + baud/2) / baud;
	if (div < 64) div = 64;
	if (div > 0x3FFFFF) div = 0x3FFFFF;
	UART0->IBRD = div >> 6;
	UART0->FBRD = div & 0x3f;
}

// get current Baud rate
u32 UART0_GetBaud(void)
{
	u32 div = UART0->IBRD*64 + UART0->FBRD;
	return (UARTClock*4 + div/2) / div;
}

// UART0 initialize
//  baud ... Baud rate 300-4000000
//  data ... data bits 5-8
//  stop2 ... False = 1 stop bit, True = 2 stop bits
//  parity ... parity UART_PARITY_*
// Setup GPIOs first.
void UART0_Init(u32 baud, int data, Bool stop2, int parity)
{
	// disable interrupts
	UART0->IMSC = 0;

	// clear pending interrupt flags
	UART0->ICR = 0x7ff;

	// setup Baud rate
	UART0_SetBad(baud);

	// setup line control register
	u32 reg = ((data - 5) << 5) | B4;	// word length, enable FIFO
	if (stop2) reg |= B3;			// 2 stop bits
	switch (parity)
	{
	case UART_PARITY_ODD:	reg |= B1;
	case UART_PARITY_EVEN:	reg |= B1 | B2;
	case UART_PARITY_SPACE:	reg |= B1 | B2 | B7;
	case UART_PARITY_MARK:	reg |= B1 | B7;
	}
	UART0->LCRH = reg;

	// setup control register - enable UART, enable TXE, enable RXE
	UART0->CR = B0 | B8 | B9;
}



// write byte to UART0



/*



// write to UART0
//  buf ... source buffer
//  num ... number of bytes 1..65535
// Returns number of sent bytes ('result < num' means error)
// Initialize UART0_Init and remap GPIOs before using.
int UART0_Write(const void* buf, int num)
{





}

*/
