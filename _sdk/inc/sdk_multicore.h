
// ****************************************************************************
//
//                               Multicore
//
// ****************************************************************************

#if USE_MULTICORE		// 1=use multicore (for applications), 0=do not use other cores (for loader)

#if RASPPI > 1

#ifndef _SDK_MULTICORE_H
#define _SDK_MULTICORE_H

#ifdef __cplusplus
extern "C" {
#endif

#if AARCH == 64

struct TSpinTable
{
	volatile uintptr_t SpinCore[CORES];
};
STATIC_ASSERT(sizeof(struct TSpinTable) == 4*8, "Incorrect struct TSpinTable!");

#define ARM_SPIN_TABLE_BASE	0x000000D8

// get pointer to spincore (of type volatile uintptr_t*)
#define SPINCORE_PTR(core) ((volatile uintptr_t*)(uintptr_t)(ARM_SPIN_TABLE_BASE + (core)*sizeof(uintptr_t)))

#endif // AARCH == 64

// core state
#define CORE_STATE_IDLE		0	// idle - waiting for command
#define CORE_STATE_BOOT		1	// core is booting
#define CORE_STATE_RUN		2	// running
#define CORE_STATE_HALT		3	// halt (exit system)

// core command
#define CORE_CMD_NONE		0	// no function
#define CORE_CMD_RUN		1	// run function "func", go to RUN state
#define CORE_CMD_STOP		2	// exit function, go to IDLE state
#define CORE_CMD_HALT		3	// halt core and stop system, go to HALT state

// run core function
typedef void (*pCoreFunc)(int core, void* arg);

// core state
typedef struct {
	volatile u32		cmd;		// command flags
	volatile u32		state;		// core state
	volatile pCoreFunc	func;		// required function to run
	volatile void*		arg;		// function argument
} sCoreState;

// state of cores
extern sCoreState CoreState[CORES];	// state of cores

// start secondary core
void StartSecondary(void);

// Multicore initialize - start cores 1..3
void MultiInit(void);

// main secondary function - entry secondary core
void MainSecondary(void);

// get core state CORE_STATE_*
INLINE int GetCoreState(int core) { dmb(); return CoreState[core].state; }

// check if core state is IDLE/RUN/HALT
INLINE Bool CoreIsIdle(int core) { return GetCoreState(core) == CORE_STATE_IDLE; }
INLINE Bool CoreIsRun(int core) { return GetCoreState(core) == CORE_STATE_RUN; }
INLINE Bool CoreIsHalt(int core) { return GetCoreState(core) == CORE_STATE_HALT; }

// get core command CORE_CMD_*
INLINE int GetCoreCmd(int core) { dmb(); return CoreState[core].cmd; }

// check core STOP reqest (function should terminate)
INLINE Bool CoreStopReq(int core) { return GetCoreCmd(core) == CORE_CMD_STOP; }

// run function on core (core must be in IDLE state)
void RunCore(int core, pCoreFunc func, void* arg);

// set request to stop the function running on the core
void StopCore(int core);

// request to halt to core (core must be in IDLE state)
void HaltCore(int core);

// waiting until the core is in IDLE state
void WaitCoreIdle(int core);

// send signal from current core to destination core
//  core ... destination core 0..3
//  signal ... signal index 0..31 (0..SIGNAL_NUM-1)
INLINE void SignalSend(int core, int signal) { LocalMailboxSet(core, CpuID(), 1 << signal); }

#ifdef __cplusplus
}
#endif

#endif // _SDK_MULTICORE_H

#endif // RASPPI > 1

#endif // USE_MULTICORE
