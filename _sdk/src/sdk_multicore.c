
// ****************************************************************************
//
//                               Multicore
//
// ****************************************************************************

#include "../../includes.h"	// includes

#if USE_MULTICORE		// 1=use multicore (for applications), 0=do not use other cores (for loader)

#if RASPPI > 1

// state of cores
sCoreState CoreState[CORES];	// state of cores

// Multicore initialize - start cores 1..3
void MultiInit(void)
{
	int i, core;
	Bool ok;

	// clear core states
	for (i = 0; i < CORES; i++)
	{
		CoreState[i].cmd = CORE_CMD_NONE;	// clear command
		CoreState[i].state = CORE_STATE_BOOT;	// state - core is booting
	}
	CoreState[0].state = CORE_STATE_RUN;	// core0 is running

	ArmLocal_t* a = ArmLocal;

	// clear data cache
	CleanDataCache();

	// waiting for cores to be ready
	do {
		ok = True;
		for (core = 1; core < CORES; core++)
		{
			dsb();
#if AARCH==32
			// read mailbox 3
			if (a->MAILBOXCLR[core*4+3] != 0) ok = False;
#else
			// use spin entry
			if (*SPINCORE_PTR(core) != 0) ok = False;
#endif
		}
	} while (!ok);

	// send destination address to cores
	for (core = 1; core < CORES; core++)
	{
#if AARCH==32
		// write mailbox 3
		a->MAILBOXSET[core*4+3] = (uintptr_t)&StartSecondary;
#else
		// write spin entry
		*SPINCORE_PTR(core) = (uintptr_t)&StartSecondary;
		CleanDataCache();	// clean data cache
		dsb();
#endif
	}

	// send signal to cores and wait for acceptance
	// - We start the cores in parallel because starting the core
	//   takes a while, so we don't have to wait too long.
	do {
		// send event signal
		sev();

		// data memory barrier
		dmb();

		ok = True;
		for (core = 1; core < CORES; core++)
		{
			dsb();

#if AARCH==32
			// read mailbox 3
			if (a->MAILBOXCLR[core*4+3] != 0) ok = False;
#else
			// invalidate data cache in range
			// - This function must be used when core0 has an active MMU cache,
			//   but the other cores do not yet have an active MMU.
			InvalidateDataCacheRange((uintptr_t)SPINCORE_PTR(core), sizeof(uintptr_t));

			// use spin entry
			if (*SPINCORE_PTR(core) != 0) ok = False;
#endif
		}
	} while (!ok);

	// wait cores to start working
	do {
		ok = True;
		for (core = 1; core < CORES; core++)
		{
			dsb();

			// check core state
			if (GetCoreState(core) == CORE_STATE_BOOT) ok = False;
		}
	} while (!ok);

	// clear all messageboxes
	LocalMailboxClr(0, 0, ~0);
	LocalMailboxClr(0, 1, ~0);
	LocalMailboxClr(0, 2, ~0);
	LocalMailboxClr(0, 3, ~0);

	// enable mailbox IRQ interrupts of this core
	a->MAILBOXINT[0] = B0|B1|B2|B3;
}

// main secondary function - entry secondary core
void MainSecondary(void)
{
	// get current core ID
	int core = CpuID();

	// clear all messageboxes
	LocalMailboxClr(core, 0, ~0);
	LocalMailboxClr(core, 1, ~0);
	LocalMailboxClr(core, 2, ~0);
#if AARCH!=32
	LocalMailboxClr(core, 3, ~0);	// AARCH==32 already cleared
#endif

	// enable mailbox IRQ interrupts of this core
	ArmLocal->MAILBOXINT[core] = B0|B1|B2|B3;

	// enable fast interrupts FIQ
	ef();

	// enable global interrupts IRQ
	ei();

	// idle loop
	sCoreState* cs = &CoreState[core];
	cs->cmd = CORE_CMD_NONE;	// clear command
	cs->state = CORE_STATE_IDLE;	// idle state
	dsb();
	while (True)
	{
		// wait for event
		wfe();
		dsb();

		// check command
		int cmd = cs->cmd;
		if (cmd != CORE_CMD_NONE)
		{
			// clear command
			cs->cmd = CORE_CMD_NONE;
			dsb();

			// run function
			if (cmd == CORE_CMD_RUN)
			{
				// run state
				cs->state = CORE_STATE_RUN;
				dsb();

				// run user function
				cs->func(core, (void*)cs->arg);

				// stop state
				cs->cmd = CORE_CMD_NONE;
				cs->state = CORE_STATE_IDLE;
				dsb();
			}

			// halt core
			else if (cmd == CORE_CMD_HALT)
			{
				// halt state
				cs->state = CORE_STATE_HALT;
				dsb();
				break;
			}
		}
	}
}

// run function on core (core must be in IDLE state)
void RunCore(int core, pCoreFunc func, void* arg)
{
	// wait for IDLE state
	WaitCoreIdle(core);

	// setup function
	sCoreState* cs = &CoreState[core];
	cs->func = func;
	cs->arg = arg;
	dsb();

	// setup command
	cs->cmd = CORE_CMD_RUN;
	dsb();

	// wake-up core from the idle loop, wait for the core to accept the command
	do {
		sev();
		dmb();
	} while (cs->cmd != CORE_CMD_NONE);
}

// set request to stop the function running on the core
void StopCore(int core)
{
	sCoreState* cs = &CoreState[core];
	cs->cmd = CORE_CMD_STOP;
	dsb();
}

// request to halt to core (core must be in IDLE state)
void HaltCore(int core)
{
	// stop core
	StopCore(core);

	// wait for IDLE state
	WaitCoreIdle(core);

	// setup command
	sCoreState* cs = &CoreState[core];
	cs->cmd = CORE_CMD_HALT;
	dsb();

	// halt core from the idle loop, wait for the core to accept the command
	do {
		sev();
		dmb();
	} while (cs->cmd != CORE_CMD_NONE);
}

// waiting until the core is in IDLE state
void WaitCoreIdle(int core)
{
	while (!CoreIsIdle(core)) {}
}

#endif // RASPPI > 1

#endif // USE_MULTICORE
