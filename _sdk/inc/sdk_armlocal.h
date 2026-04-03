
// ****************************************************************************
//
//                               ARM Locals
//
// ****************************************************************************
// These core-local peripherals are not available on Zero1 and Pi1 models.
//
// Note - The values of the "System Timer" and "Generic Timer" counters are
// synchronized at system startup so that their time value is equivalent.
// However, values from timers should not be confused with each other.
// After a long period of time, the time value may drift and differ.
//
// Local peripherals of the core contain:
// - 64-bit incrementing core timer "Generic Timer" (this timer is common for all cores)
//	4 channels of Generic Timer:
//		CNTPS - Secure Physical Timer (Counter Non-virtualized Timer Physical Secure) ... used in EL3 (cannot be used)
//		CNTPNS - Non-Secure Physical Timer (Counter Non-virtualized Timer Physical NS-Non-Secure) ... used in EL1 (preferably used timer in this kernel)
//		CNTHP - Hypervisor Physical Timer (Counter Non-virtualized Timer Hypervisor Physical) ... used in EL2 (cannot be used)
//		CNTV - Virtual Timer (Counter Non-virtualized Timer Virtual) .. used in EL0 (user mode - usualy not used in this kernel)
// - 28-bit decrementing "Local Timer"
// - 4 ARM local mailboxes
//		Mailbox 3 is used when starting the secondary kernel.
//		Mailboxes 1 and 2 are available for use, e.g., for IPC (Inter-Core Communication), but are not typically used.
//		Mailbox 0 is typically used for communication between kernels (IPI Inter-Processor Interrupts).
//		A non-zero mailbox value activates the pending bit for interrupts from the mailbox.
//
// Crystal clock:
//	Pi 1, 2, 3, Zero: 19.2 MHz
//	Pi 4, 400, 5, CM4: 54 MHz
//
// Timer divider = 2^31 / prescaler_value .... (prescaler_value <= 0x80000000)
// If prescaler_value = 0x80000000 -> divider ratio = 1
// Setting prescaler to 0 will stop the timer.
// To get divider ratio of 19.2 use: 2^31/19.2 = 111848107 = 0x06AAAAAB (with error 8.9e-9)
// To get divider ratio of 54 use: 2^31/54 = 39768216 = 0x025ED098

#if RASPPI > 1

#ifndef _SDK_ARMLOCAL_H
#define _SDK_ARMLOCAL_H

#define IRQ_LOCAL_FIRST		72	// first local IRQ

// ARM core local peripherals
typedef struct {

	io32	TIMERCTRL;	// 0x00: control register of 64-bit core timer "Generic Timer" (selects timer source for all 4 cores)
				//	bit 8:	0 = 64-bit core timer runs from the crystal clock (19.2 or 54 MHz)
				//		1 = 64-bit core timer runs from the APB clock
				//			APB clock is running at half speed of ARM clock
				//	bit 9:	0 = 64-bit core timer increments by 2
				//		1 = 64-bit core timer increments by 1

	io32	res;		// 0x04: ... reserved (deprecated: timer prescaler subtract)

	io32	TIMERPRESC;	// 0x08: 64-bit core timer "Generic Timer" prescaler 1..0x80000000, 0=stop the timer
				//	timer_frequency = (prescaler / 2^31) * input_frequency
				// To get divider ratio of 19.2 use: 2^31/19.2 = 111848107 = 0x06AAAAAB (with error 8.9e-9), result frequency 1.00000000298 MHz
				// To get divider ratio of 54 use: 2^31/54 = 39768216 = 0x025ED098, result frequency 1.00000000745 MHz

	io32	GPUROUT;	// 0x0C: GPU interrupts routing (to one core only)
				//	bit 0,1: GPU IRQ routing to core 0..3
				//	bit 2,3: GPU FIQ routing to core 0..3

	io32	PMROUTSET;	// 0x10: performance monitor unit (PMU) interrupts routing - set (write "1" bit to set)
	io32	PMROUTCLR;	// 0x14: performance monitor unit (PMU) interrupts routing - clear (write "1" bit to clear)
				//   Selects nPMUIRQ routing signal.
				//   PMU from a certain core can only generate interrupts
				//   to that core, not to any of the other cores.
				//   If bit 4..7 is enabled, then bit 0..3 is ignored
				//	bit 0: nPMUIRQ[0] IRQ control 0=IRQ disabled, 1=IRQ enabled (ignored if bit 4 is set)
				//	bit 1: nPMUIRQ[1] IRQ control 0=IRQ disabled, 1=IRQ enabled (ignored if bit 5 is set)
				//	bit 2: nPMUIRQ[2] IRQ control 0=IRQ disabled, 1=IRQ enabled (ignored if bit 6 is set)
				//	bit 3: nPMUIRQ[3] IRQ control 0=IRQ disabled, 1=IRQ enabled (ignored if bit 7 is set)
				//	bit 4: nPMUIRQ[0] FIQ control 0=FIQ disabled, 1=FIQ enabled (bit 0 is ignored if set)
				//	bit 5: nPMUIRQ[1] FIQ control 0=FIQ disabled, 1=FIQ enabled (bit 1 is ignored if set)
				//	bit 6: nPMUIRQ[2] FIQ control 0=FIQ disabled, 1=FIQ enabled (bit 2 is ignored if set)
				//	bit 7: nPMUIRQ[3] FIQ control 0=FIQ disabled, 1=FIQ enabled (bit 3 is ignored if set)

	io32	res2;		// 0x18: ... reserved

	io32	TIMERLS;	// 0x1C: 64-bit core timer "Generic Timer" LS less significant 32 bits
				//		to read: read LS and then MS
				//		to write: write LS and then MS

	io32	TIMERMS;	// 0x20: 64-bit core timer "Generic Timer" MS more significant 32 bits
				//		to read: read LS and then MS
				//		to write: write LS and then MS

	io32	INTROUT;	// 0x24: local interrupt routing
				//	bit 0..2: "Local Timer" interrupt goes to:
				//		0 = core 0 IRQ
				//		1 = core 1 IRQ
				//		2 = core 2 IRQ
				//		3 = core 3 IRQ
				//		4 = core 0 FIQ
				//		5 = core 1 FIQ
				//		6 = core 2 FIQ
				//		7 = core 3 FIQ

	io32	res3;		// 0x28: ... reserved

	io32	AXICOUNT;	// 0x2C: AXI outstanding counters (read only)
				//	bit 0..9: outstanding reads counter
				//	bit 15..16: outstanding writes counter

	io32	AXIIRQ;		// 0x30: AXI outstanding interrupt
				//	bit 0..19: AXI outstanding time-out MS 24 bits
				//	bit 20: 1=AXI outstanding interrupt enable

	io32	LOCTIMER;	// 0x34: "Local Timer" control and status (raises interrupt on value 0)
				//   Counting-down local timer always gets its timing pulses from crystal clock.
				//   Timer is shifted every clock edge - for 19.2 MHz it will get 38.4 Mpulses/sec.
				//	bit 0..27: re-load value (28-bit programmable divider)
				//	bit 28: 1=timer enable
				//	bit 31: 1=interrupt pending = counter reaches 0 and reloaded (read only)

	io32	LOCTIMERCLR;	// 0x38: "Local Timer" clear and reload (write-only)
				//	bit 30: 1=reload local timer (without generating interrupt - can be used as watchdog)
				//	bit 31: 1=clear interrupt flag

	io32	res4;		// 0x3C: ... reserved

	io32	TIMERINT[4];	// 0x40 (16 = 0x10): core 0..3 "Local Timer" interrupt control (CNTPNS is preferably used timer in this kernel)
				//   If bit 4..7 is enabled, then bit 0..3 is ignored
				//	bit 0: nCNTPSIRQ IRQ control 0=IRQ disabled, 1=IRQ enabled (ignored if bit 4 is set)
				//	bit 1: nCNTPNSIRQ IRQ control 0=IRQ disabled, 1=IRQ enabled (ignored if bit 5 is set)
				//	bit 2: nCNTHPIRQ IRQ control 0=IRQ disabled, 1=IRQ enabled (ignored if bit 6 is set)
				//	bit 3: nCNTVIRQ IRQ control 0=IRQ disabled, 1=IRQ enabled (ignored if bit 7 is set)
				//	bit 4: nCNTPSIRQ FIQ control 0=FIQ disabled, 1=FIQ enabled (bit 0 is ignored if set)
				//	bit 5: nCNTPNSIRQ FIQ control 0=FIQ disabled, 1=FIQ enabled (bit 1 is ignored if set)
				//	bit 6: nCNTHPIRQ FIQ control 0=FIQ disabled, 1=FIQ enabled (bit 2 is ignored if set)
				//	bit 7: nCNTVIRQ FIQ control 0=FIQ disabled, 1=FIQ enabled (bit 3 is ignored if set)

	io32	MAILBOXINT[4];	// 0x50 (16 = 0x10): core 0..3 mailbox interrupt control
				//  - A non-zero mailbox value activates the pending bit for interrupts from the mailbox.
				//   If bit 4..7 is enabled, then bit 0..3 is ignored
				//	bit 0: Mailbox-0 IRQ control 0=IRQ disabled, 1=IRQ enabled (ignored if bit 4 is set)
				//	bit 1: Mailbox-1 IRQ control 0=IRQ disabled, 1=IRQ enabled (ignored if bit 5 is set)
				//	bit 2: Mailbox-2 IRQ control 0=IRQ disabled, 1=IRQ enabled (ignored if bit 6 is set)
				//	bit 3: Mailbox-3 IRQ control 0=IRQ disabled, 1=IRQ enabled (ignored if bit 7 is set)
				//	bit 4: Mailbox-0 FIQ control 0=FIQ disabled, 1=FIQ enabled (bit 0 is ignored if set)
				//	bit 5: Mailbox-1 FIQ control 0=FIQ disabled, 1=FIQ enabled (bit 1 is ignored if set)
				//	bit 6: Mailbox-2 FIQ control 0=FIQ disabled, 1=FIQ enabled (bit 2 is ignored if set)
				//	bit 7: Mailbox-3 FIQ control 0=FIQ disabled, 1=FIQ enabled (bit 3 is ignored if set)

	io32	IRQPEND[4];	// 0x60 (16 = 0x10): core 0..3 IRQ interrupt pending source
				//	bit 0: 1=CNTPSIRQ interrupt pending
				//	bit 1: 1=CNTPNSIRQ interrupt pending (CNTPNS is preferably used timer in this kernel)
				//	bit 2: 1=CNTHPIRQ interrupt pending
				//	bit 3: 1=CNTVIRQ interrupt pending
				//	bit 4: 1=Mailbox 0 interrupt pending (mailbox has non-zero content)
				//	bit 5: 1=Mailbox 1 interrupt pending (mailbox has non-zero content)
				//	bit 6: 1=Mailbox 2 interrupt pending (mailbox has non-zero content)
				//	bit 7: 1=Mailbox 3 interrupt pending (mailbox has non-zero content)
				//	bit 8: 1=GPU interrupt pending (can be high in one core only)
				//	bit 9: 1=PMU interrupt pending (performance monitor unit)
				//	bit 10: 1=AXI-outstanding interrupt pending (for core 0 only, all others are 0)
				//	bit 11: 1=local timer interrupt pending
				//	bit 12..17: peripheral 1..15 interrupt pending (currently not used)

	io32	FIQPEND[4];	// 0x70 (16 = 0x10): core 0..3 FIQ fast inrerrupt pending source
				//	bit 0: 1=CNTPSIRQ fast interrupt pending
				//	bit 1: 1=CNTPNSIRQ fast interrupt pending (CNTPNS is preferably used timer in this kernel)
				//	bit 2: 1=CNTHPIRQ fast interrupt pending
				//	bit 3: 1=CNTVIRQ fast interrupt pending
				//	bit 4: 1=Mailbox 0 fast interrupt pending (mailbox has non-zero content)
				//	bit 5: 1=Mailbox 1 fast interrupt pending (mailbox has non-zero content)
				//	bit 6: 1=Mailbox 2 fast interrupt pending (mailbox has non-zero content)
				//	bit 7: 1=Mailbox 3 fast interrupt pending (mailbox has non-zero content)
				//	bit 8: 1=GPU fast interrupt pending (can be high in one core only)
				//	bit 9: 1=PMU fast interrupt pending
				//	bit 10: always 0
				//	bit 11: 1=local timer fast interrupt pending
				//	bit 12..17: peripheral 1..15 fast interrupt pending (currently not used)

	// set mailbox - write to set bits, send message to the core (mailbox 3 is used)
	//  - Writing bits with a value of "1" sets the mailbox bits to "1" - i.e., an "OR" operation.
	//  - All cores have equal access to registers; there is no distinction between which core performs which operation.
	//  - A non-zero mailbox value activates the pending bit for interrupts from the mailbox.
	union {
		struct {
			io32	MAILBOXSET0[4];	// 0x80 (16 = 0x10): core 0 mailbox 0..3 register - set (no read, only write "1" to set - send message)
			io32	MAILBOXSET1[4];	// 0x90 (16 = 0x10): core 1 mailbox 0..3 register - set (no read, only write "1" to set - send message)
			io32	MAILBOXSET2[4];	// 0xA0 (16 = 0x10): core 2 mailbox 0..3 register - set (no read, only write "1" to set - send message)
			io32	MAILBOXSET3[4];	// 0xB0 (16 = 0x10): core 3 mailbox 0..3 register - set (no read, only write "1" to set - send message)
		};
		io32	MAILBOXSET[16];	// 0x80 (64 = 0x40): core 0..3 mailbox 0..3 register - set (no read, only write "1" to set - send message)
					//	0..3: core 0 mailbox 0..3
					//	4..7: core 1 mailbox 0..3
					//	8..11: core 2 mailbox 0..3
					//	12..15: core 3 mailbox 0..3, 
	};

	// read/clear mailbox - read to receive message, write to clear bits
	//  - Writing bits with a value of "1" clears the mailbox bits to "0" - i.e., an "AND NOT" operation.
	//  - All cores have equal access to registers; there is no distinction between which core performs which operation.
	//  - A non-zero mailbox value activates the pending bit for interrupts from the mailbox.
	union {
		struct {
			io32	MAILBOXCLR0[4];	// 0xC0 (16 = 0x10): core 0 mailbox 0..3 register - read/clear (read, or write "1" to clear)
			io32	MAILBOXCLR1[4];	// 0xD0 (16 = 0x10): core 1 mailbox 0..3 register - read/clear (cleared by write)
			io32	MAILBOXCLR2[4];	// 0xE0 (16 = 0x10): core 2 mailbox 0..3 register - read/clear (cleared by write)
			io32	MAILBOXCLR3[4];	// 0xF0 (16 = 0x10): core 3 mailbox 0..3 register - read/clear (cleared by write)
		};
		io32	MAILBOXCLR[16];	// 0xC0 (64 = 0x40): core 0..3 mailbox 0..3 register - read/clear (read, or write "1" to clear)
					//	0..3: core 0 mailbox 0..3
					//	4..7: core 1 mailbox 0..3
					//	8..11: core 2 mailbox 0..3
					//	12..15: core 3 mailbox 0..3, 
	};

} ArmLocal_t;
STATIC_ASSERT(sizeof(ArmLocal_t) == 0x100, "Incorrect ArmLocal_t!");

//  ARM core local peripherals interface
#define ArmLocal	((ArmLocal_t*)ARM_LOCAL_BASE)

// ---- Generic Timer (This timer is started at system startup, with 1 us ticks)

// Initialize Generic Timer - set clock to 1 MHz and 1 us tick (this function is called when the system starts up)
void GenericTimerInit(void);

// Get LOW value of Generic Timer counter - time in [us] from start of the system
// - On Pi1/Zero1 use function Time() from SysTimer
//  Note - The values of the "System Timer" and "Generic Timer" counters are
//  synchronized at system startup so that their time value is equivalent.
//  However, values from timers should not be confused with each other.
//  After a long period of time, the time value may drift and differ.
INLINE u32 Time(void)
{
	// read virtual counter
#if AARCH==32
	u32 lo, hi;
	__asm volatile (" mrrc p15,1,%0,%1,c14\n" : "=r" (lo), "=r" (hi));
	return lo;
#else
	u64 val;
	__asm volatile (" mrs %0,cntvct_el0\n" : "=r" (val));
	return (u32)val;
#endif
}

// Get value of Generic Timer counter - time in [us] from start of the system
// - On Pi1/Zero1 use function Time64() from SysTimer
//  Note - The values of the "System Timer" and "Generic Timer" counters are
//  synchronized at system startup so that their time value is equivalent.
//  However, values from timers should not be confused with each other.
//  After a long period of time, the time value may drift and differ.
INLINE u64 Time64(void)
{
	// read virtual counter
#if AARCH==32
	u32 lo, hi;
	__asm volatile (" mrrc p15,1,%0,%1,c14\n" : "=r" (lo), "=r" (hi));
	return ((u64)hi << 32 | lo);
#else
	u64 val;
	__asm volatile (" mrs %0,cntvct_el0\n" : "=r" (val));
	return val;
#endif
}

// disable Generic Timer interrupts (it only disables interrupts from compare register, not Generic Timer itself)
INLINE void GenericTimerStop(void)
{
#if AARCH==32
	// CNTP_CTL: disable timer
	__asm volatile ( "mcr p15,0,%0,c14,c2,1\n" :: "r"(0));
#else
	// CNTP_CTL_EL0: disable timer
	__asm volatile (" msr cntp_ctl_el0,%0\n" :: "r"(0));
#endif
}

// start one-shot timeout in [us] (raises interrupt after given timeout in [us])
// - To generate periodic tick, call GenericTimerTimeout() again from interrupt handler.
INLINE void GenericTimerTimeout(u32 us)
{
#if AARCH==32
	// CNTP_TVAL = usec
	__asm volatile (" mcr p15,0,%0,c14,c2,0\n" :: "r"(us));

	// CNTP_CTL = enable (bit0), irq enable (bit1)
	u32 ctl = 1 | (1 << 1);
	__asm volatile (" mcr p15,0,%0,c14,c2,1\n" :: "r"(ctl));
#else
	// CNTP_TVAL_EL0 = usec
	__asm volatile (" msr cntp_tval_el0,%0\n" :: "r"(us));

	// CNTP_CTL_EL0 = enable + irq enable
	u64 ctl = 1 | (1 << 1);
	__asm volatile (" msr cntp_ctl_el0,%0\n" :: "r"(ctl));
#endif
}

// raise interrupt at absolute time in [us]
INLINE void GenericTimerAlarm(u64 time)
{
#if AARCH==32
	u32 lo = (u32)time;
	u32 hi = (u32)(time >> 32);

	// CNTP_CVAL = 64bit target
	__asm volatile (" mcrr p15,2,%0,%1,c14\n" :: "r"(lo), "r"(hi));

	// enable + irq
	u32 ctl = 1 | (1 << 1);
	__asm volatile (" mcr p15,0,%0,c14,c2,1\n" :: "r"(ctl));
#else
	__asm volatile (" msr cntp_cval_el0,%0\n" :: "r"(time));

	u64 ctl = 1 | (1 << 1);
	__asm volatile (" msr cntp_ctl_el0,%0\n" :: "r"(ctl));
#endif
}

// set bits in mailbox (write bit "1" to set bit)
INLINE void LocalMailboxSet(int core, int mailbox, u32 set) { ArmLocal->MAILBOXSET[core*4+mailbox] = set; }

// clear bits in mailbox (write bit "1" to clear bit)
INLINE void LocalMailboxClr(int core, int mailbox, u32 clr) { ArmLocal->MAILBOXCLR[core*4+mailbox] = clr; }

// read mailbox
INLINE u32 LocalMailboxGet(int core, int mailbox) { return ArmLocal->MAILBOXCLR[core*4+mailbox]; }

// enable/disable mailbox IRQ interrupt
//   If both IRQ and FIQ interrupts are enabled, the FIQ interrupt takes precedence.
INLINE void LocalMailboxIRQEnable(int core, int mailbox) { ArmLocal->MAILBOXINT[core] |= (1 << mailbox); }
INLINE void LocalMailboxIRQDisable(int core, int mailbox) { ArmLocal->MAILBOXINT[core] &= ~(1 << mailbox); }

// enable/disable mailbox FIQ fast interrupt
//   If both IRQ and FIQ interrupts are enabled, the FIQ interrupt takes precedence.
INLINE void LocalMailboxFIQEnable(int core, int mailbox) { ArmLocal->MAILBOXINT[core] |= (1 << (mailbox+4)); }
INLINE void LocalMailboxFIQDisable(int core, int mailbox) { ArmLocal->MAILBOXINT[core] &= ~(1 << (mailbox+4)); }

// check if local IRQ interrupt is pending (use IRQ_LOCAL_* constants; IRQ values are in range 72..83)
INLINE Bool LocalIRQPending(int core, int irq) { return ArmLocal->IRQPEND[core] & (1 << (irq - IRQ_LOCAL_FIRST)); }

// check if local FIQ fast interrupt is pending (use IRQ_LOCAL_* constants; IRQ values are in range 72..83)
INLINE Bool LocalFIQPending(int core, int irq) { return ArmLocal->FIQPEND[core] & (1 << (irq - IRQ_LOCAL_FIRST)); }

#endif // _SDK_ARMLOCAL_H

#endif // RASPPI > 1
