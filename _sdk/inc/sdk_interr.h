
// ****************************************************************************
//
//                            Interrupt controller
//
// ****************************************************************************
// The global disable di() function only disables normal IRQ interrupts, but not
// fast FIQ interrupts. To disable FIQ interrupts, you must use the DisableFIQ()
// function - but this is not recommended, because FIQ is a fast interrupt that
// requires immediate handling. An FIQ interrupt can also be triggered during
// IRQ handling, but only if FIQ interrupts are enabled.

// During interrupt handling, certain system functions are not allowed to be used
// - such as memory allocator, printf output. These functions allow sharing between
// processor cores, but not sharing with interrupts.

// Float and double operations can be used during interrupts - coprocessor
// registers are completely preserved in the interrupt handler.

// Interrupts from peripherals can only be handled in core 0. Other cores only
// allow interrupts from the ARM local timer CNTPNS and interrupts when a message
// arrives via ARM local mailbox.

#ifndef _SDK_INTERR_H
#define _SDK_INTERR_H

#ifdef __cplusplus
extern "C" {
#endif

// IRQ interrupt indices
#define IRQ_TIMER0		0	// System timer channel 0
#define IRQ_TIMER1		1	// System timer channel 1
#define IRQ_TIMER2		2	// System timer channel 2
#define IRQ_TIMER3		3	// System timer channel 3
#define IRQ_CODEC0		4	// CODEC 0
#define IRQ_CODEC1		5	// CODEC 1
#define IRQ_CODEC2		6	// CODEC 2
#define IRQ_JPEG		7	// JPEG
#define IRQ_ISP			8	// ISP
#define IRQ_USB			9	// USB
#define IRQ_3D			10	// 3D
#define IRQ_TRANSPOSER		11
#define IRQ_MULTICORESYNC0	12
#define IRQ_MULTICORESYNC1	13
#define IRQ_MULTICORESYNC2	14
#define IRQ_MULTICORESYNC3	15
#define IRQ_DMA0		16
#define IRQ_DMA1		17
#define IRQ_DMA2		18
#define IRQ_DMA3		19
#define IRQ_DMA4		20
#define IRQ_DMA5		21
#define IRQ_DMA6		22
#define IRQ_DMA7		23
#define IRQ_DMA8		24
#define IRQ_DMA9		25
#define IRQ_DMA10		26
#define IRQ_DMA11		27	// shared DMA channels 11-14
#define IRQ_DMA_SHARED		28	// unused shared irq for all DMA channels
#define IRQ_AUX			29	// Aux
#define IRQ_ARM			30
#define IRQ_VPUDMA		31

#define IRQ_HOSTPORT		32
#define IRQ_VIDEOSCALER		33
#define IRQ_CCP2TX		34
#define IRQ_SDC			35
#define IRQ_DSI0		36
#define IRQ_AVE			37
#define	IRQ_CAM0		38
#define IRQ_CAM1		39
#define IRQ_HDMI0		40
#define IRQ_HDMI1		41
#define IRQ_PIXELVALVE1		42
#define IRQ_I2CSPISLV		43	// I2C SPI slave
#define IRQ_DSI1		44
#define IRQ_PWA0		45	// PWA0
#define IRQ_PWA1		46	// PWA1
#define IRQ_CPR			47
#define IRQ_SMI			48	// SMI
#define IRQ_GPIO0		49	// GPIO0
#define IRQ_GPIO1		50	// GPIO1
#define IRQ_GPIO2		51	// GPIO2
#define IRQ_GPIO3		52	// GPIO3
#define IRQ_I2C			53	// I2C
#define IRQ_SPI			54	// SPI
#define IRQ_I2SPCM		55	// PCM
#define IRQ_SDIO		56
#define IRQ_UART		57	// UART
#define IRQ_SLIMBUS		58
#define IRQ_VEC			59
#define IRQ_CPG			60
#define IRQ_RNG			61
#define IRQ_ARASANSDIO		62
#define IRQ_AVPSMON		63

#define IRQ_ARMTIMER		64	// ARM timer
#define IRQ_MAILBOX		65	// ARM Mailbox
#define IRQ_DOORBELL0		66	// ARM Doorbell 0
#define IRQ_DOORBELL1		67	// ARM Doorbell 1
#define IRQ_GPU0HALT		68	// GPU0 (or GPU1) halted
#define IRQ_GPU1HALT		69	// GPU1 halted
#define IRQ_ILLEGAL1		70	// Illegal access type 1 (address/access error from address bit 26..29)
#define IRQ_ILLEGAL0		71	// Illegal access type 0 (address/access error from address bit 30..31)

#if RASPPI > 1
#define	IRQ_LOCAL_CNTPS		72	// CNTPS - Secure Physical Timer (Counter Non-virtualized Timer Physical Secure) ... used in EL3 (cannot be used)
#define IRQ_LOCAL_CNTPNS	73	// CNTPNS - Non-Secure Physical Timer (Counter Non-virtualized Timer Physical NS-Non-Secure) ... used in EL1 (preferably used timer in this kernel)
#define IRQ_LOCAL_CNTHP		74	// CNTHP - Hypervisor Physical Timer (Counter Non-virtualized Timer Hypervisor Physical) ... used in EL2 (cannot be used)
#define IRQ_LOCAL_CNTV		75	// CNTV - Virtual Timer (Counter Non-virtualized Timer Virtual) .. used in EL0 (user mode - usualy not used in this kernel)
#define IRQ_LOCAL_MAILBOX0	76	// ARM local mailbox 0 interrupt (mailbox has non-zero content)
#define IRQ_LOCAL_MAILBOX1	77	// ARM local mailbox 1 interrupt (mailbox has non-zero content)
#define IRQ_LOCAL_MAILBOX2	78	// ARM local mailbox 2 interrupt (mailbox has non-zero content)
#define IRQ_LOCAL_MAILBOX3	79	// ARM local mailbox 3 interrupt (mailbox has non-zero content)
#define IRQ_LOCAL_GPU		80	// cascade GPU interrupts (can be enable only in one core only)
#define IRQ_LOCAL_PMU		81	// performance monitor unit (PMU) interrupt
#define IRQ_LOCAL_AXI_IDLE	82	// AXI-outstanding interrupt (on core 0 only)
#define IRQ_LOCAL_LOCALTIMER	83	// local timer interrupt

#define IRQ_LOCAL_FIRST		72	// first local IRQ

#define IRQ_NUM			84	// number of IRQs

#else // RASPPI > 1

#define IRQ_NUM			72	// number of IRQs

#endif // RASPPI > 1

#define IRQ_NUMPEND		72	// number of supported pending IRQs

#if RASPPI > 1
// inter-processor interrupt (IPI), sent via mailbox as bit mask
#define SIGNAL_NUM		32	// number of IPI signals
#endif // RASPPI > 1

// Interrupt controller
typedef struct {

	io32	PEND;		// 0x00: (read-only) IRQ basic pending register
				//		bit 0..7: IRQ 64..71 (IRQ_ARMTIMER..IRQ_ILLEGAL1) is pending
				//		bit 8: 1=one or more bits set in pending register 1, except IRQs connected to basic pending register
				//		bit 9: 1=one or more bits set in pending register 2, except IRQs connected to basic pending register
				//		bit 10: IRQ 7 ... IRQ_JPEG
				//		bit 11: IRQ 9 ... IRQ_USB
				//		bit 12: IRQ 10 ... IRQ_3D
				//		bit 13: IRQ 18 ... IRQ_DMA2
				//		bit 14: IRQ 19 ... IRQ_DMA3
				//		bit 15: IRQ 53 ... IRQ_I2C
				//		bit 16: IRQ 54 ... IRQ_SPI
				//		bit 17: IRQ 55 ... IRQ_I2SPCM
				//		bit 18: IRQ 56 ... IRQ_SDIO
				//		bit 19: IRQ 57 ... IRQ_UART
				//		bit 20: IRQ 62 ... IRQ_ARASANSDIO
	io32	PEND1;		// 0x04: IRQ pending source 0..31
	io32	PEND2;		// 0x08: IRQ pending source 32..63

	io32	FIQ;		// 0x0C: FIQ control - index of one selected interrupt (FIQ has higher pirority than other IRQ)
				//	bit 0..6: select FIQ source IRQ_*
				//	bit 7: 1 = enable FIQ

	io32	EN1;		// 0x10: Enable IRQs 1: write 1 to enable IRQ source 0..31, other IRQs are unaffected
	io32	EN2;		// 0x14: Enable IRQs 2: write 1 to enable IRQ source 32..63, other IRQs are unaffected
	io32	EN;		// 0x18: Enable basic IRQs: write 1 to enable basic interrupt 64..83

	io32	DIS1;		// 0x1C: Disable IRQs 1: write 1 to disable IRQ source 0..31, other IRQs are unaffected
	io32	DIS2;		// 0x20: Disable IRQs 2: write 1 to disable IRQ source 32..63, other IRQs are unaffected
 	io32	DIS;		// 0x24: Disable basic IRQs: write 1 to disable basic interrupt 64..83

} IntC_t;

STATIC_ASSERT(sizeof(IntC_t) == 0x28, "Incorrect IntC_t!");

// Interrupt controller interface
#define IntC	((IntC_t*)ARM_INTC_BASE)

// interrupt handler type
typedef void tIntHandler(void);

// exception table (located at address 0x00000000)
typedef struct {
	u32	reset;
	u32	undefined;
	u32	supervisor;
	u32	prefetchabort;
	u32	dataabort;
	u32	unused;
	u32	irq;
	u32	fiq;
} ExceptionTable_t;

STATIC_ASSERT(sizeof(ExceptionTable_t) == 0x20, "Incorrect ExceptionTable_t!");

// list of interrupt handlers
extern tIntHandler* IntHandlerList[IRQ_NUM];

// address of FIQ handler
extern tIntHandler* FIQHandlerAddr;

#if RASPPI > 1
// Signal interrupt handler type
//  core ... destination core 0..3
//  from ... source core 0..3
//  signal ... signal index 0..31 (0..SIGNAL_NUM-1)
typedef void tSignalHandler(int core, int from, int signal);

// list of signal handlers (32 signals to 4 destination core = 128 handlers total)
extern tSignalHandler* SignalHandlerList[SIGNAL_NUM*CORES];
#endif // RASPPI > 1

// initialize interrupt system (called from SysInit)
void IntInit();

// terminate interrupt system
void IntTerm();

// Enable interrupt (irq = interrupt index IRQ_*)
// Before enabling interrupts, first set up the interrupt handler using IntHandler().
void IntEnable(int irq);

// Disable interrupt (irq = interrupt index IRQ_*)
void IntDisable(int irq);

// Check if interrupt is pending (return True if pending, irq = interrupt index IRQ_*)
Bool IntPending(int irq);

// setup IRQ interrupt handler
// After setting up the interrupt handler, enable interrupt with IntEnable().
// Interrupts from peripherals can only be handled in core 0. Other cores only
// allow interrupts from the ARM local timer CNTPNS and interrupts when a message
// arrives via ARM local mailbox.
INLINE void IntHandler(int irq, tIntHandler* handler) { cb(); IntHandlerList[irq] = handler; dsb(); }

// start FIQ service - set handler and enable FIQ
void FIQStart(int irq, tIntHandler* handler);

// stop FIQ service
INLINE void FIQStop() { cb(); IntC->FIQ = 0; cb(); }

#if RASPPI > 1
// setup signal interrupt handler, with destination core entered
//  core ... destination core 0..3
//  signal ... signal index 0..31 (0..SIGNAL_NUM-1)
//  handler ... pointer to handler function
INLINE void SignalHandlerCore(int core, int signal, tSignalHandler* handler)
{
	cb();
	SignalHandlerList[core*SIGNAL_NUM + signal] = handler;
	dsb();
}

// setup signal interrupt handler for current core
//  signal ... signal index 0..31 (0..SIGNAL_NUM-1)
//  handler ... pointer to handler function
INLINE void SignalHandler(int signal, tSignalHandler* handler)
{
	cb();
	SignalHandlerList[CpuID()*SIGNAL_NUM + signal] = handler;
	dsb();
}
#endif

// disable all interrupts
void IntDisableAll();

#ifdef __cplusplus
}
#endif

#endif // _SDK_INTERR_H
