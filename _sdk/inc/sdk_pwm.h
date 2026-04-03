
// ****************************************************************************
//
//                       PWM - Pulse Width Modulator
//
// ****************************************************************************
// - 2 independent output bit-streams
// - FIFO 16 (or 8?) words 32-bit (or 64 words on Pi4)
// - clocked by clk_pwm 100 MHz (see Clock Manager CPRMAN)
// - can be used to serialised output of 32-bit word

// PWM is mapped to DMA request 5 (and 1 on Pi4).
// #define DREQ_PWM	5
// #if RASPPI >= 4
// #define DREQ_DSI0_PWM1		1	// DSI0 or PWM1 - selected in DMA_CNTRL_MUX
// #endif

// Modes:
//   1) PWM mode N/M algorithm (MODE=0 MSEN=0): "1" and "0" bits are spread out as evenly as possible (DAC style output)
//	algorithm, period M cycles, value N cycles "1", M-N cycles "0": 
//		1. Set context = 0
//		2. context = context + N
//		3. if (context >= M)
//			context = context - M
//			send 1
//		   else
//			send 0
//		4. Repeat from step 2
//  2) PWM mode M/S (MODE=0 MSEN=1): simple PWM ratio with 1st pulse M cycles "1" width, 2nd pulse S-M cycles "0" width
//  3) Serializer (MODE=1) - 32-bit data are sent serially

/*
GPIO alternate functions, RASPPI <= 3
------------------------------------
	Pull	ALT0	ALT5
GPIO12	Low	PWM0	-
GPIO13	Low	PWM1	-
GPIO18	Low	-	PWM0
GPIO19	Low	-	PWM1
GPIO40	Low	PWM0	-
GPIO41	Low	PWM1	-
GPIO45	-	PWM1	-

GPIO alternate functions, RASPPI >= 4
------------------------------------
	Pull	ALT0	ALT5
GPIO12	Low	PWM0_0	-
GPIO13	Low	PWM0_1	-
GPIO18	Low	-	PWM0_0
GPIO19	Low	-	PWM0_1
GPIO40	Low	PWM1_0	-
GPIO41	Low	PWM1_1	-
GPIO45	-	PWM0_1	-
*/

#ifndef _SDK_PWM_H
#define _SDK_PWM_H

// PWM Pulse Width Modulator
typedef struct {
	io32	CTL;		// 0x00: control
				//	bit 0: "PWEN1" channel 1: 1=enabled, 0=disabled
				//	bit 1: "MODE1" channel 1 mode: 1=serialiser, 0=PWM (using PWM or MS/S transmission)
				//	bit 2: "RPTL1" channel 1 repeat last data: 1=repeat last data if FIFO is empty, 0=stop on FIFO empty
				//	bit 3: "SBIT1" channel 1 silence: state of output when no transmission
				//	bit 4: "POLA1" channel 1 polarity: 1=1-low 0-high, 0=0-low 1-high
				//	bit 5: "USEF1" channel 1 use FIFO for transmission: 1=FIFO used, 0=data register transmitted
				//	bit 6: "CLRF" (write-only) write "1" to clear FIFO (read as 0)
				//	bit 7: "MSEN1" channel 1 PWM M/S enable: 1=M/S simple transmission used, 0=N/M algorithm used
				//	bit 8: "PWEN2" channel 2: 1=enabled, 0=disabled
				//	bit 9: "MODE2" channel 2 mode: 1=serialiser, 0=PWM (using PWM or MS/S transmission)
				//	bit 10: "RPTL2" channel 2 repeat last data: 1=repeat last data if FIFO is empty, 0=stop on FIFO empty
				//	bit 11: "SBIT2" channel 2 silence: state of output when no transmission
				//	bit 12: "POLA2" channel 2 polarity: 1=1-low 0-high, 0=0-low 1-high
				//	bit 13: "USEF2" channel 2 use FIFO for transmission: 1=FIFO used, 0=data register transmitted
				//	bit 15: "MSEN2" channel 2 PWM M/S enable: 1=M/S simple transmission used, 0=N/M algorithm used

	io32	STA;		// 0x04: status
				//	bit 0: "FULL1" (read-only) FIFO is full
				//	bit 1: "EMPT1" (read-only) FIFO is empty
				//	bit 2: "WERR1" FIFO write error (FIFO was full when write), write "1" to clear
				//	bit 3: "RERR1" FIFO read error (FIFO was empty when read), write "1" to clear
				//	bit 4: "GAPO1" channel 1 gap occured flag (gap between transmissions), write "1" to clear
				//	bit 5: "GAPO2" channel 2 gap occured flag (gap between transmissions), write "1" to clear
				//	bit 8: "BERR" bus error flag, write "1" to clear
				//	bit 9: "STA1" (read-only) channel 1 state: 1=channel is transmitting data
				//	bit 10: "STA2" (read-only) channel 2 state: 1=channel is transmitting data

	io32	DMAC;		// 0x08: DMA configuration
				//	bit 0..7: "DREQ" DMA threshold for DREQ signal (7 default)
				//	bit 8..15: "PANIC" DMA threshold for PANIC signal (7 default)
				//	bit 31: "ENAB" 1=DMA enabled, 0=DMA disabled

	io32	res;		// 0x0C: ... reserved

	io32	RNG1;		// 0x10: channel 1 range - length of PWM period in pulses (default 32)
				//		serial mode: data are truncated (<32) or padded (>32)

	io32	DAT1;		// 0x14: channel 1 data - data to be sent to PWM when USEF=0

	io32	FIF1;		// 0x18: FIFO input - store data to FIFO, send to PWM when USEF=1
				//	If both USEF are enabled, FIFO is shared and data are sent interleaved
				//		1 2 3 4 5 6 7 8 -> ch1 1 3 5 7, ch2 2 4 6 8
				//		 ... RNG should be the same, RPTL should be 0
				//	If the set of channels sharing the FIFO has been modified after a
				//	configuration change, the FIFO should be cleared before writing new data.

	io32	res2;		// 0x1C: ... reserved

	io32	RNG2;		// 0x20: channel 2 range - length of PWM period in pulses (default 32)
				//		serial mode: data are truncated (<32) or padded (>32)

	io32	DAT2;		// 0x24: channel 2 data - data to be sent to PWM when USEF=0
} PWM_t;
STATIC_ASSERT(sizeof(PWM_t) == 0x28, "Incorrect PWM_t!");

// PWM interface
#define PWM ((PWM_t*)ARM_PWM_BASE)
#if RASPPI == 4
#define PWM1 ((PWM_t*)ARM_PWM1_BASE)
#endif

// start PWM output, 2 channels
//  freq ... Required frequency in [Hz] - GPIOCLOCK_SRC_PLLD
//		clock source with integer division will be used
//  period ... PWM period in number of clocks
//  msmode ... use simple M/S mode (or use spread out N/M mode otherwise - recommended for audio)
//  invert ... invert output
void PWM_Start(u32 freq, int period, Bool msmode, Bool invert);

// stop PWM output
void PWM_Stop(void);

#endif // _SDK_PWM_H
