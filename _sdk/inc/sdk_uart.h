
// ****************************************************************************
//
//                                  UART0
//
// ****************************************************************************
// - PL011 UART0
// - DMA and IrDA not supported
// - 16 x 8-bit transmit FIFO, 16 x 12-bit receive FIFO
// - CTS, RTS hardware flow control
// - 5, 6, 7 or 8 data bits
// - even, odd, stick, or no-parity bit generation and detection
// - 1 or 2 stop bit
// - UART clock source are controlled by Clock Manager
// - FIFO trigger levels 1/8, 1/4, 1/2, 3/4 and 7/8

#ifndef _SDK_UART_H
#define _SDK_UART_H

#ifdef __cplusplus
extern "C" {
#endif

// UART0 structure
typedef struct
{
	io32	DR;		// 0x00: data register
				//  16 x 8-bit transmit FIFO, 16 x 12-bit receive FIFO
				//	bit 0-7: "DATA" receive/transmit data character
				//		For received words:
				//		if the FIFOs are enabled, the data byte and
				//		the 4-bit status (break, frame, parity, and overrun)
				//		 is pushed onto the 12-bit wide receive FIFO
				//		if the FIFOs are not enabled, the data byte and status
				//		are stored in the receiving holding register (the bottom
				//		word of the receive FIFO).
				//	bit 8: (read only) "FE" framing error
				//		When set to 1, it indicates
				//		that the received character did not have a
				//		valid stop bit (a valid
				//		stop bit is 1). In FIFO mode, this error is
				//		associated with the character at the top of the FIFO.
				//	bit 9: (read only) "PE" parity error
				//		When set to 1, it indicates that
				//		the parity of the received data character
				//		does not match the
				//		parity that the EPS and SPS bits in the Line
				//		Control Register, UART_LCRH select. In
				//		FIFO mode, this error is associated with the
				//		character at the top of the FIFO.
				//	bit 10: (read only) "BE" break error
				//		This bit is set to 1 if a break
				//		condition was detected, indicating that the
				//		received data input
				//		was held LOW for longer than a full-word
				//		transmission time (defined as start, data,
				//		parity and stop bits).
				//		In FIFO mode, this error is associated with
				//		the character at the top of the FIFO. When
				//		a break occurs,
				//		only one 0 character is loaded into the
				//		FIFO. The next character is only enabled
				//		after the receive data
				//		input goes to a 1 (marking state), and the
				//		next valid start bit is received.
				//	bit 11: (read only) "OE" overrun error
				//		This bit is set to 1 if data is
				//		received and the receive FIFO is already full.
				//		This is cleared to 0 once there is an empty
				//		space in the FIFO and a new character can
				//		be written to it.

	io32	RSRECR;		// 0x04: receive status register/error clear
				//	bit 0: "FE" framing error
				//		When set to 1, it indicates
				//		that the received character did not have a
				//		valid stop bit (a valid
				//		stop bit is 1). In FIFO mode, this error is
				//		associated with the character at the top of the FIFO.
				//	bit 1: "PE" parity error
				//		When set to 1, it indicates that
				//		the parity of the received data character
				//		does not match the
				//		parity that the EPS and SPS bits in the Line
				//		Control Register, UART_LCRH select. In
				//		FIFO mode, this error is associated with the
				//		character at the top of the FIFO.
				//	bit 2: "BE" break error
				//		This bit is set to 1 if a break
				//		condition was detected, indicating that the
				//		received data input
				//		was held LOW for longer than a full-word
				//		transmission time (defined as start, data,
				//		parity and stop bits).
				//		In FIFO mode, this error is associated with
				//		the character at the top of the FIFO. When
				//		a break occurs,
				//		only one 0 character is loaded into the
				//		FIFO. The next character is only enabled
				//		after the receive data
				//		input goes to a 1 (marking state), and the
				//		next valid start bit is received.
				//	bit 3: "OE" overrun error
				//		This bit is set to 1 if data is
				//		received and the receive FIFO is already full.
				//		This is cleared to 0 once there is an empty
				//		space in the FIFO and a new character can
				//		be written to it.

	io32	res[4];		// 0x08: ... reserved

	io32	FR;		// 0x18: flag register (read only)
				//	bit 0: "CTS" clear to send
				//		This bit is the complement of
				//		the UART clear to send, nUARTCTS,
				//		modem status input. That is, the bit is 1
				//		when nUARTCTS is LOW.
				//	bit 1: "DSR" ... unsupported
				//	bit 2: "DCD" ... unsupported
				//	bit 3: "BUSY" UART busy
				//		If this bit is set to 1, the UART
				//		is busy transmitting data. This bit remains
				//		set until the complete byte, including all the
				//		stop bits, has been sent from the shift register.
				//		This bit is set as soon as the transmit FIFO
				//		becomes non-empty, regardless of whether
				//		the UART is enabled or not.
				//	bit 4: "RXFE" Receive FIFO empty
				//		The meaning of this
				//		bit depends on the state of the FEN bit in
				//		the UARTLCR_H Register.
				//		If the FIFO is disabled, this bit is set when
				//		the receive holding register is empty.
				//		If the FIFO is enabled, the RXFE bit is set
				//		when the receive FIFO is empty.
				//	bit 5: "TXFF" Transmit FIFO full
				//		The meaning of this bit
				//		depends on the state of the FEN bit in the
				//		UARTLCR_ LCRH Register.
				//		If the FIFO is disabled, this bit is set when
				//		the transmit holding register is full.
				//		If the FIFO is enabled, the TXFF bit is set
				//		when the transmit FIFO is full.
				//	bit 6: "RXFF" Receive FIFO full
				//		The meaning of this bit
				//		depends on the state of the FEN bit in the
				//		UARTLCR_ LCRH Register.
				//		If the FIFO is disabled, this bit is set when
				//		the receive holding register is full.
				//		If the FIFO is enabled, the RXFF bit is set
				//		when the receive FIFO is full.
				//	bit 7: "TXFE" Transmit FIFO empty
				//		The meaning of this
				//		bit depends on the state of the FEN bit in
				//		the Line Control Register, UARTLCR_LCRH.
				//		If the FIFO is disabled, this bit is set when
				//		the transmit holding register is empty.
				//		If the FIFO is enabled, the TXFE bit is set
				//		when the transmit FIFO is empty. This bit
				//		does not indicate if there is data in the
				//		transmit shift register.
				//	bit 8: "RI" ... unsupported

	io32	res2;		// 0x1C: ... reserved

	io32	ILPR;		// 0x20: not used (disabled IrDA register)

	io32	IBRD;		// 0x24: integer Baud rate divisor, bit 0-15: "IBRD"
				// NOTE: The UART_LCRH, UART_IBRD, and UART_FBRD registers must no be changed:
				// - when the UART is enabled
				// - when completing a transmission or a reception
				// - when it has been programmed to become disabled.
				// NOTE: The contents of the IBRD and FBRD registers are not updated until
				//    transmission or reception of the current character is complete.

	io32	FBRD;		// 0x28: fractional Baud rate divisor, bit 0-5: "FBRD"
				// NOTE: The UART_LCRH, UART_IBRD, and UART_FBRD registers must no be changed:
				// - when the UART is enabled
				// - when completing a transmission or a reception
				// - when it has been programmed to become disabled.
				//	The baud rate divisor is calculated as follows:
				//	Baud rate divisor BAUDDIV = (FUARTCLK/(16 * Baud_rate))
				//	where FUARTCLK is the UART reference clock frequency. The BAUDDIV is
				//	comprised of the integer value IBRD and the fractional value FBRD.
				// NOTE: The contents of the IBRD and FBRD registers are not updated until
				//    transmission or reception of the current character is complete.

	io32	LCRH;		// 0x2C: line control register
				// NOTE: The UART_LCRH, UART_IBRD, and UART_FBRD registers must no be changed:
				// - when the UART is enabled
				// - when completing a transmission or a reception
				// - when it has been programmed to become disabled.
				//	bit 0: "BRK" send break
				//		If this bit is set to 1, a low-level
				//		is continually output on the TXD output,
				//		after completing transmission of the current character.
				//	bit 1: "PEN" parity enable
				//		0 = parity is disabled and no parity bit
				//		added to the data frame
				//		1 = parity checking and generation is enabled.
				//	bit 2: "EPS" even parity select
				//		Controls the type of
				//		parity the UART uses during transmission
				//		and reception:
				//		0 = odd parity. The UART generates or
				//		checks for an odd number of 1s in the data
				//		and parity bits.
				//		1 = even parity. The UART generates or
				//		checks for an even number of 1s in the
				//		data and parity bits.
				//		This bit has no effect when the PEN bit
				//		disables parity checking and generation.
				//	bit 3: "STP2" two stop bits select
				//		If this bit is set to 1,
				//		two stop bits are transmitted at the end of
				//		the frame. The receive
				//		logic does not check for two stop bits being received.
				//	bit 4: "FEN" enable FIFOs:
				//		0 = FIFOs are disabled (character mode)
				//		that is, the FIFOs become 1-byte-deep
				//		holding registers
				//		1 = transmit and receive FIFO buffers are
				//		enabled (FIFO mode).
				//	bit 5-6: "WLEN" word length
				//		0 = 5 bits
				//		1 = 6 bits
				//		2 = 7 bits
				//		3 = 8 bits
				//	bit 7: "SPS" stick parity select
				//		0 = stick parity is disabled
				//		1 = either:
				//		if the EPS bit is 0 then the parity bit is
				//		transmitted and checked as a 1
				//		if the EPS bit is 1 then the parity bit is
				//		transmitted and checked as a 0. 

	io32	CR;		// 0x30: control register
				// NOTE: To enable transmission, the TXE bit and UARTEN bit
				//  must be set to 1. Similarly, to enable reception, the RXE
				//  bit and UARTEN bit, must be set to 1.
				// NOTE: Program the control registers as follows:
				//  1. Disable the UART.
				//  2. Wait for the end of transmission or reception
				//     of the current character.
				//  3. Flush the transmit FIFO by setting the FEN bit
				//     to 0 in the Line Control Register, UART_LCRH.
				//  4. Reprogram the Control Register, UART_CR.
				//  5. Enable the UART.
				//	bit 0: "UARTEN" 1=UART enable
				//	bit 1: "SIREN" ... unsupported
				//	bit 2: "SIRLP" ... unsupported
				//	bit 7: "LBE" 1=loopback enable
				//	bit 8: "TXE" 1=transmit enable
				//	bit 9: "RXE" 1=receive enable
				//	bit 10: "DTR" ... unsupported
				//	bit 11: "RTS" 1=request to send (nUARTRTS is LOW)
				//	bit 12: "OUT1" ... unsupported
				//	bit 13: "OUT2" ... unsupported
				//	bit 14: "RTSEN" 1=RTS hardware flow control enable
				//	bit 15: "CTSEN" 1=CTS hardware flow control enable

	io32	IFLS;		// 0x34: interrupt FIFO level select register
				//	bit 0-2: "TXIFLSEL" transmit interrupt FIFO level select
				//		The trigger points for the transmit interrupt are as follows:
				//		0 = Transmit FIFO becomes 1/8 full
				//		1 = Transmit FIFO becomes 1/4 full
				//		2 = Transmit FIFO becomes 1/2 full
				//		3 = Transmit FIFO becomes 3/4 full
				//		4 = Transmit FIFO becomes 7/8 full
				//		5,6,7 = reserved.
				//	bit 3-5: "RXIFLSEL" receive interrupt FIFO level select
				//		The trigger points for the receive interrupt are as follows:
				//		0 = Receive FIFO becomes 1/8 full
				//		1 = Receive FIFO becomes 1/4 full
				//		2 = Receive FIFO becomes 1/2 full
				//		3 = Receive FIFO becomes 3/4 full
				//		4 = Receive FIFO becomes 7/8 full
				//		5,6,7 = reserved.
				//	bit 6-8: "TXIFPSEL" ... unsupported
				//	bit 9-11: "RXIFPSEL" ... unsupported

	io32	IMSC;		// 0x38: interrupt mask set clear register (1=interrupt is enabled)
				//	bit 0: "RIMIM" ... unsupported
				//	bit 1: "CTSMIM" nUARTCTS modem interrupt mask
				//	bit 2: "DCDMUM" ... unsupported
				//	bit 3: "DSRMUM" ... unsupported
				//	bit 4: "RXIM" receive interrupt mask
				//	bit 5: "TXIM" transmit interrupt mask
				//	bit 6: "RTIM" receive timeout interrupt mask
				//	bit 7: "FEIM" framing error interrupt mask
				//	bit 8: "PEIM" parity error interrupt mask
				//	bit 9: "BEIM" break error interrupt mask
				//	bit 10: "OEIM" overrun error interrupt mask

	io32	RIS;		// 0x3C: raw interrupt status register (1=interrupt is pending)
				//	bit 0: "RIRMIS" ... unsupported
				//	bit 1: "CTSRMIS" CTS modem interrupt state
				//	bit 2: "DCDRMIS" ... unsupported
				//	bit 3: "DSRRMIS" ... unsupported
				//	bit 4: "RXRIS" receive interrupt state
				//	bit 5: "TXRIS" transmit interrupt state
				//	bit 6: "RTRIS" receive timeout interrupt state
				//	bit 7: "FERIS" framing error interrupt state
				//	bit 8: "PERIS" parity error interrupt state
				//	bit 9: "BERIS" break error interrupt status
				//	bit 10: "OERIS" overrun error interrupt status

	io32	MIS;		// 0x40: masked interrupt status register (1=interrupt is pending and enabled)
				//	bit 0: "RIMMIS" ... unsupported
				//	bit 1: "CTSMMIS" CTS modem interrupt state
				//	bit 2: "DCDMMIS" ... unsupported
				//	bit 3: "DSRMMIS" ... unsupported
				//	bit 4: "RXMIS" receive interrupt state
				//	bit 5: "TXMIS" transmit interrupt state
				//	bit 6: "RTMIS" receive timeout interrupt state
				//	bit 7: "FEMIS" framing error interrupt state
				//	bit 8: "PEMIS" parity error interrupt state
				//	bit 9: "BEMIS" break error interrupt status
				//	bit 10: "OEMIS" overrun error interrupt status

	io32	ICR;		// 0x44: interrupt clear regiter (write 1 to clear interrupt flag)
				//	bit 0: "RIMIC" ... unsupported
				//	bit 1: "CTSMIC" CTS modem interrupt
				//	bit 2: "DCDMIC" ... unsupported
				//	bit 3: "DSRMIC" ... unsupported
				//	bit 4: "RXIC" receive interrupt
				//	bit 5: "TXIC" transmit interrupt
				//	bit 6: "RTIC" receive timeout interrupt
				//	bit 7: "FEIC" framing error interrupt
				//	bit 8: "PEIC" parity error interrupt
				//	bit 9: "BEIC" break error interrupt
				//	bit 10: "OEIC" overrun error interrupt

	io32	DMACR;		// 0x48: DMA control register ... unsupported
				//	bit 0: "RXDMAE" ... unsupported
				//	bit 1: "TXDMAE" ... unsupported
				//	bit 2: "DMAONERR" ... unsupported

	io32	res3[13];	// 0x4C: ... reserved

	io32	ITCR;		// 0x80: test control register
				//	bit 0: "ITCR0" Integration test enable. When this bit is 1,
				//		the UART is placed in integration test
				//		mode, otherwise it is in normal operation.
				//	bit 1: "ITCR1" Test FIFO enable. When this bit it 1, a write
				//		to the Test Data Register, UART_DR writes
				//		data into the receive FIFO, and reads from
				//		the UART_DR register reads data out of
				//		the transmit FIFO.
				//		When this bit is 0, data cannot be read
				//		directly from the transmit FIFO or written
				//		directly to the receive FIFO (normal operation).

	io32	ITIP;		// 0x84: integration test input register
				//	bit 0: "ITIP0" Reads return the value of the UARTRXD primary input.
				//	bit 3: "ITIP3" Reads return the value of the nUARTCTS primary input.

	io32	ITOP;		// 0x88: integration test output register
				//	bit 0: "ITIP0" Primary output. Writes specify the value to be driven on UARTTXD.
				//	bit 3: "ITIP3" Primary output. Writes specify the value to be driven on nUARTRTS.
				//	bit 6: "ITIP6" Intra-chip output. Writes specify the value
				//		to be driven on UARTINTR.
				//		Reads return the value of UARTINTR at the
				//		output of the test multiplexor.
				//	bit 7: "ITOP7" Intra-chip output. Writes specify the value
				//		to be driven on UARTEINTR.
				//		Reads return the value of UARTEINTR at
				//		the output of the test multiplexor.
				//	bit 8: "ITOP8" Intra-chip output. Writes specify the value
				//		to be driven on UARTRTINTR.
				//		Reads return the value of UARTRTINTR at
				//		the output of the test multiplexor.
				//	bit 9: "ITOP9" Intra-chip output. Writes specify the value
				//		to be driven on UARTTXINTR.
				//		Reads return the value of UARTTXINTR at
				//		the output of the test multiplexor.
				//	bit 10: "ITOP10" Intra-chip output. Writes specify the value
				//		to be driven on UARTRXINTR.
				//		Reads return the value of UARTRXINTR at
				//		the output of the test multiplexor.
				//	bit 11: "ITOP11" Intra-chip output. Writes specify the value
				//		to be driven on UARTMSINTR.
				//		Reads return the value of UARTMSINTR at
				//		the output of the test multiplexor.

	io32	TDR;		// 0x8C: test data register
				//	bit 0-10: "TDR10_0" When the ITCR1 bit is set to 1, data is
				//		written into the receive FIFO and read out
				//		of the transmit FIFO.

} UART_t;
STATIC_ASSERT(sizeof(UART_t) == 0x90, "Incorrect UART_t!");

// UART0 peripheral
#define UART0	((UART_t*)ARM_UART0_BASE)

// GPIOs:
//		Pull	ALT0	ALT2	ALT3
// GPIO14	Low	TXD0	-	-
// GPIO15	Low	RXD0	-	-
// GPIO16	Low	-	-	CTS0
// GPIO17	Low	-	-	RTS0
// *GPIO30	Low	-	-	CTS0	Bluetooth
// *GPIO31	Low	-	-	RTS0	Bluetooth
// *GPIO32	Low	-	-	TXD0	Bluetooth
// *GPIO33	Low	-	-	RXD0	Bluetooth
// *GPIO36	High	-	TXD0	-
// *GPIO37	Low	-	RXD0	-
// *GPIO38	Low	-	RTS0	-
// *GPIO39	Low	-	CTS0	-
//  ... only pins GPIO14-GPIO17 are accessible on extern connector

// parity select
#define UART_PARITY_NONE	0	// no parity
#define UART_PARITY_ODD		1	// odd parity
#define UART_PARITY_EVEN	2	// even parity
#define UART_PARITY_SPACE	3	// parity bit is 0
#define UART_PARITY_MARK	4	// parity bit is 1

// set Baud rate
// Baud rate must no be changed:
// - when the UART is enabled
// - when completing a transmission or a reception
// - when it has been programmed to become disabled.
void UART0_SetBad(u32 baud);

// get current Baud rate
u32 UART0_GetBaud(void);

// UART0 initialize
//  baud ... Baud rate 300-4000000
//  data ... data bits 5-8
//  stop2 ... False = 1 stop bit, True = 2 stop bits
//  parity ... parity UART_PARITY_*
// Setup GPIOs first.
void UART0_Init(u32 baud, int data, Bool stop2, int parity);

// check if can write next character
INLINE Bool UART0_CanWrite(void) { return (UART0->FR & B5) == 0; } // flag TXFF, transmit FIFO is full

// check if can read next character
INLINE Bool UART0_CanRead(void) { return (UART0->FR & B4) == 0; } // flag RXFE, receive FIFO is empty



#ifdef __cplusplus
}
#endif

#endif // _SDK_UART_H
