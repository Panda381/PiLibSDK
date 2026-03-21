
// ****************************************************************************
//
//                            Interrupt controller
//
// ****************************************************************************

#include "../../includes.h"	// includes

// get dinstance from "ldr pc,.." instruction to handler address
#define ARM_DISTANCE(from, to)		((u32*)&(to) - (u32*)&(from) - 2)

// get "ldr pc,..." instruction vode
#define ARM_OPCODE_BRANCH(distance)	(0xEA000000 | (distance))

// list of interrupt handlers
tIntHandler* IntHandlerList[IRQ_NUM];

// address of FIQ handler - this handler is called from FIQ exception handler FIQHandler at startup32.S
tIntHandler* FIQHandlerAddr;

#if RASPPI > 1
// list of signal handlers (8 signals to 4 destination core = 32 handlers total)
tSignalHandler* SignalHandlerList[SIGNAL_NUM*CORES];
#endif // RASPPI > 1

// initialize interrupt system (called from SysInit)
void IntInit()
{

#if AARCH==32

extern void UndefinedHandler(void);	// undefined exception handler
extern void IRQHandler(void);		// IRQ handler
extern void FIQHandler(void);		// FIQ handler

	// initialize exception table
	ExceptionTable_t* volatile e = (ExceptionTable_t* volatile)0;
	e->undefined = ARM_OPCODE_BRANCH(ARM_DISTANCE(e->undefined, UndefinedHandler));
	e->supervisor = ARM_OPCODE_BRANCH(ARM_DISTANCE(e->supervisor, UndefinedHandler));
	e->prefetchabort = ARM_OPCODE_BRANCH(ARM_DISTANCE(e->prefetchabort, UndefinedHandler));
	e->dataabort = ARM_OPCODE_BRANCH(ARM_DISTANCE(e->dataabort, UndefinedHandler));
	e->unused = ARM_OPCODE_BRANCH(ARM_DISTANCE(e->unused, UndefinedHandler));
	e->irq = ARM_OPCODE_BRANCH(ARM_DISTANCE(e->irq, IRQHandler));
	e->fiq = ARM_OPCODE_BRANCH(ARM_DISTANCE(e->fiq, FIQHandler));

	// flush cache
	SyncDataAndInstructionCache();
#endif

	// disable all interrupts
	IntDisableAll();
}

// terminate interrupt system
void IntTerm()
{
	// disable all interrupts
	IntDisableAll();
}

// Enable interrupt (irq = interrupt index IRQ_*)
// Before enabling interrupts, first set up the interrupt handler using IntHandler().
void IntEnable(int irq)
{
	if (irq < 32)
		IntC->EN1 = BIT(irq);
	else if (irq < 64)
		IntC->EN2 = BIT(irq - 32);
	else 
		IntC->EN = BIT(irq - 64);
}

// Disable interrupt (irq = interrupt index IRQ_*)
void IntDisable(int irq)
{
	if (irq < 32)
		IntC->DIS1 = BIT(irq);
	else if (irq < 64)
		IntC->DIS2 = BIT(irq - 32);
	else 
		IntC->DIS = BIT(irq - 64);
}

// Check if interrupt is pending (return True if pending, irq = interrupt index IRQ_*)
Bool IntPending(int irq)
{
	if (irq < 32)
		return (IntC->PEND1 & BIT(irq)) != 0;
	else if (irq < 64)
		return (IntC->PEND2 & BIT(irq - 32)) != 0;
	else
		return (IntC->PEND & BIT(irq - 64)) != 0;
}

// start FIQ service - set handler and enable FIQ
void FIQStart(int irq, tIntHandler* handler)
{
	// disable FIQ
	IntC->FIQ = 0;
	cb();

	// setup FIQ handler
	FIQHandlerAddr = handler;

	// enable FIQ
	cb();
	IntC->FIQ = irq | B7;
}

// disable all interrupts
void IntDisableAll()
{
	IntC->FIQ = 0;
	IntC->DIS = (u32)-1;
	IntC->DIS2 = (u32)-1;
	IntC->DIS1 = (u32)-1;
}

// IRQ interrupt dispatch - this function is called from IRQ exception handler IRQHandler at startup32.S
void IRQ_Dispatch()
{
	int i, m, p;
	u32 msg;
	tIntHandler** h = IntHandlerList;

#if RASPPI > 1
	// get index of current processor core (0..3)
	int core = CpuID();

	// ARM local CNTPNS timer interrupt
	if (LocalIRQPending(core, IRQ_LOCAL_CNTPNS))
	{
		h[IRQ_LOCAL_CNTPNS]();
	}

	// inter-core signals
	int from;
	for (from = 0; from < 4; from++)
	{
		if (LocalIRQPending(core, IRQ_LOCAL_MAILBOX0 + from))
		{
			// read mailbox
			u32 msg = LocalMailboxGet(core, from);
			if (msg != 0)
			{
				for (i = 0; i < SIGNAL_NUM; i++)
				{
					// mask
					m = 1 << i;

					// check this IPI index
					if ((msg & m) != 0)
					{
						// clear messagebox flag
						LocalMailboxClr(core, from, m);
						dsb();

						// handle this interrupt
						SignalHandlerList[core*SIGNAL_NUM+i](core, from, i);
					}
				}
			}
		}
	}

	// peripheral IRQs are serviced only on core0
	if (core != 0) return;
#endif

	// interrupts 0..31
	u32 pend = IntC->PEND1;
	if (pend != 0)
	{
		for (i = 4; i > 0; i--)
		{
			p = pend & 0xff;
			if (p != 0)
			{
				// process 8 pendings in 1 byte
				if ((p & B0) != 0) (h[0])();
				if ((p & B1) != 0) (h[1])();
				if ((p & B2) != 0) (h[2])();
				if ((p & B3) != 0) (h[3])();
				if ((p & B4) != 0) (h[4])();
				if ((p & B5) != 0) (h[5])();
				if ((p & B6) != 0) (h[6])();
				if ((p & B7) != 0) (h[7])();
			}
			h += 8;
			pend >>= 8;
		}
	}
	else
		h += 32;

	// interrupts 32..63
	pend = IntC->PEND2;
	if (pend != 0)
	{
		for (i = 4; i > 0; i--)
		{
			p = pend & 0xff;
			if (p != 0)
			{
				// process 8 pendings in 1 byte
				if ((p & B0) != 0) (h[0])();
				if ((p & B1) != 0) (h[1])();
				if ((p & B2) != 0) (h[2])();
				if ((p & B3) != 0) (h[3])();
				if ((p & B4) != 0) (h[4])();
				if ((p & B5) != 0) (h[5])();
				if ((p & B6) != 0) (h[6])();
				if ((p & B7) != 0) (h[7])();
			}
			h += 8;
			pend >>= 8;
		}
	}
	else
		h += 32;

	// interrupts 64..71
	p = IntC->PEND & 0xff;
	if (p != 0)
	{
		// process 8 pendings in 1 byte
		if ((p & B0) != 0) (h[0])();
		if ((p & B1) != 0) (h[1])();
		if ((p & B2) != 0) (h[2])();
		if ((p & B3) != 0) (h[3])();
		if ((p & B4) != 0) (h[4])();
		if ((p & B5) != 0) (h[5])();
		if ((p & B6) != 0) (h[6])();
		if ((p & B7) != 0) (h[7])();
	}
}
