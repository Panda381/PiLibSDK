
// ****************************************************************************
//
//                         Startup code, 32-bit mode
//
// ****************************************************************************
// Some parts of the code were taken from the Circle library:
/*
 * startup.S
 *
 * Circle - A C++ bare metal environment for Raspberry Pi
 * Copyright (C) 2014-2021  R. Stange <rsta2@o2online.de>
 *
 * This file contains code taken from Linux:
 *	safe_svcmode_maskall macro
 *	defined in arch/arm/include/asm/assembler.h
 *	Copyright (C) 1996-2000 Russell King
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Registers:
// - Registers available to modify without push: R0-R3, R12 (IP)
// - Registers need to be saved: R4-R11, R13 (SP), R14 (LR)

#include "config.h"

/*
 * Helper macro to enter SVC mode cleanly and mask interrupts. reg is
 * a scratch register for the macro to overwrite.
 *
 * This macro is intended for forcing the CPU into SVC mode at boot time.
 * you cannot return to the original mode.
 */
	.macro safe_svcmode_maskall reg:req

	// check if running in hypervisor mode
	mrs	\reg,cpsr			// load CPU status register (CPU mode: SVC/HYP/IRQ/FIQ..., status bits, interrupt mask)
	eor	\reg,\reg,#0x1A			// XOR to check hypervisor mode (HYP has code 0x1A)
	tst	\reg,#0x1F			// test running mode bits (result is 0 if hypervisor mode)

	// if CPU is in hypervisor mode, set back SVC mode. Disable IRQ and FIQ
	bic	\reg,\reg,#0x1F			// clear running mode bits               
	orr	\reg,\reg,#(0x13|(1<<6)|(1<<7))	// set SVC mode, disable FIQ, disable IRQ
	bne	1f				// skip if was not hypervisor mode       

	// ARM7/8 instructions to exit hypervisor mode
	orr	\reg,\reg,#0x100		// disable Abort                                                     
	adr	lr,2f				// get address of "2:" label to skip following instructions
	msr	spsr_cxsf,\reg			// set new running mode                                              
	.word	0xE12EF30E			// "msr ELR_hyp,lr" Set address of "2?" label to ELR_hyp   
	.word	0xE160006E			// "eret" Elevated return which will exit at "2:" address in SVC_MODE

	// Continue if was not in hypervisor mode
1:	msr	cpsr_c,\reg			// set new running mode
2:
	.endm

// ---------------------------------------------------------------------------
//                 Start after reset (boot at address 0x8000)
// ---------------------------------------------------------------------------

	.section .init	// init code section
.global _start
_start:

// ---- clear BSS segment

	ldr	r0,=__bss_start		// R0 <- BSS start address
	ldr	r1,=__bss_end		// R1 <- BSS end address
	mov	r2,#0			// R0 <- 0 clear word
1:	cmp	r0,r1			// reached end?
	bge	2f			// skip if end reached
	str	r2,[r0],#4		// store 0, advance R0 pointer by 4 bytes
	b	1b			// loop
2:
// ---- Exit hypervisor mode, enter SVC mode

	safe_svcmode_maskall r0

// ---- Reset VBAR (Vector Base Address Register) to address 0, if changed by u-boot

	mov	r0,#0
	mcr	p15,0,r0,c12,c0,0	// reset VBAR (if changed by u-boot)

// ---- Setup stack pointers

	// set ABORT stack and UNDEFINED stack
	ldr	r1,=__stack_end		// prepare stack end of last core
	cps	#0x17			// select ABORT mode
	mov	sp,r1			// set ABORT stack
	cps	#0x1b			// select UNDEFINED mode
	mov	sp,r1			// set UNDEFINED stack
	sub	r1,#INT_STACK_SIZE	// skip ABORT stack

	// set FIQ stack
	cps	#0x11			// select FIQ mode
	mov	sp,r1			// set FIQ stack
	sub	r1,#INT_STACK_SIZE	// skip FIQ stack

	// set IRQ stack
	cps	#0x12			// select IRQ mode
	mov	sp,r1			// set IRQ stack
	sub	r1,#INT_STACK_SIZE	// skip IRQ stack

	// set system stack, select System mode
	cps	#0x1f			// select system mode
	mov	sp,r1			// set system stack

	// jump to system initialize
	b	SysInit

.ltorg

// ---------------------------------------------------------------------------
//                        Start secondary core
// ---------------------------------------------------------------------------

#if USE_MULTICORE		// 1=use multicore (for applications), 0=do not use other cores (for loader)
#if RASPPI > 1

	.section .init	// init code section
	.align	2
.global StartSecondary
StartSecondary:

// ---- Exit hypervisor mode, enter SVC mode

	safe_svcmode_maskall r0

// ---- Setup stack pointers

	// prepare end of stack of this core
	mrc	p15,#0,r2,c0,c0,#5	// R2 <- read core ID (register MPIDR)
	and	r2,#CORES-1		// mask core ID
	ldr	r1,=__stack_end		// prepare stack end of last core
	mov	r0,#CORE_STACK_SIZE	// R0 <- stack size of one core
	mul	r2,r2,r0		// R2 <- offset of the stack
	sub	r1,r2			// R1 <- end of stack of this core

	// set ABORT stack and UNDEFINED stack
	cps	#0x17			// select ABORT mode
	mov	sp,r1			// set ABORT stack
	cps	#0x1b			// select UNDEFINED mode
	mov	sp,r1			// set UNDEFINED stack
	sub	r1,#INT_STACK_SIZE	// skip ABORT stack

	// set FIQ stack
	cps	#0x11			// select FIQ mode
	mov	sp,r1			// set FIQ stack
	sub	r1,#INT_STACK_SIZE	// skip FIQ stack

	// set IRQ stack
	cps	#0x12			// select IRQ mode
	mov	sp,r1			// set IRQ stack
	sub	r1,#INT_STACK_SIZE	// skip IRQ stack

	// set system stack, select System mode
	cps	#0x1f			// select system mode
	mov	sp,r1			// set system stack

	// jump to core initialize
	b	SysInitSecondary

.ltorg

#endif // RASPPI > 1
#endif // USE_MULTICORE

// ---------------------------------------------------------------------------
//                          Exception handlers
// ---------------------------------------------------------------------------

	.section .init	// init code section
	.align	2
.global UndefinedHandler
UndefinedHandler:
	b	.

// IRQ exception handler (RAM requirements to save all registers - about 288 bytes, minimal interrupt time about 250ns)
	.align	2
.global IRQHandler
IRQHandler:

	// prepare return address
	// - LR points behind the instruction where the interruption occurred;
	//   we need to return to the beginning of the instruction.
	sub	lr,lr,#4			// lr: return address

	// save base registers R0-R3, R12, LR
	stmfd	sp!,{r0-r3,r12,lr}		// save r0-r3, r12 and return address

	// save VFP registers
	sub	sp,sp,#4			// correct stack (number of pushes must be even)
	vmrs	r0,fpscr			// R0 <- get FPSCR status register
	stmfd	sp!,{r0}			// store R0 with FPSCR to the stack
	vstmdb	sp!,{d0-d15}			// save VFP registers D0-D15
#if RASPPI >= 2
	vstmdb	sp!,{d16-d31}			// save VFP registers D16-D31
#endif

	// process interrupt - function is located at sdk_interr.c
	bl	IRQ_Dispatch

	// restore VFP registers
#if RASPPI >= 2
	vldmia	sp!,{d16-d31}			// restore VFP registers D16-D31
#endif
	vldmia	sp!,{d0-d15}			// restore VFP registers D0-D15
	ldmfd	sp!,{r0}			// R0 <- restore FPSCR from stack
	vmsr	fpscr,r0			// restore FPSCR status register
	add	sp,sp,#4			// correct stack back

	// restore registers R0-R3, R12, PC (return)
	ldmfd	sp!,{r0-r3,r12,pc}^		// restore registers and return (^ = restore CPSR from SPSR_irq)

.ltorg


// FIQ exception handler (RAM requirements to save all registers - about 288 bytes, delay about 200ns)
	.align	2
.global FIQHandler
FIQHandler:

	// prepare return address
	// - LR points behind the instruction where the interruption occurred;
	//   we need to return to the beginning of the instruction.
	sub	lr,lr,#4			// lr: return address

	// save base registers R0-R3, R12, LR
	stmfd	sp!,{r0-r3,r12,lr}		// save r0-r3, r12 and return address

	// save VFP registers
	sub	sp,sp,#4			// correct stack (number of pushes must be even)
	vmrs	r0,fpscr			// R0 <- get FPSCR status register
	stmfd	sp!,{r0}			// store R0 with FPSCR to the stack
	vstmdb	sp!,{d0-d15}			// save VFP registers D0-D15
#if RASPPI >= 2
	vstmdb	sp!,{d16-d31}			// save VFP registers D16-D31
#endif

	// BCM2835 - data memory barrier, to see memory access in right order
#if RASPPI == 1
	mov	r3,#0
	mcr	p15,0,r3,c7,c10,5		// data memory barrier
#endif

	// process interrupt
	ldr	r2,=FIQHandlerAddr		// R2 <- pointer to address of FIQ handler
	ldr	r1,[r2]				// R1 <- FIQ handler
	blx	r1				// call FIQ handler

	// BCM2835 - data synchronization barrier, wait until all memory operations are complete
#if RASPPI == 1
	mov	r3,#0
	mcr	p15,0,r3,c7,c10,4		// data synchronization barrier
#endif

	// restore VFP registers
#if RASPPI >= 2
	vldmia	sp!,{d16-d31}			// restore VFP registers D16-D31
#endif
	vldmia	sp!,{d0-d15}			// restore VFP registers D0-D15
	ldmfd	sp!,{r0}			// R0 <- restore FPSCR from stack
	vmsr	fpscr,r0			// restore FPSCR status register
	add	sp,sp,#4			// correct stack back

	// restore registers R0-R3, R12, PC (return)
	ldmfd	sp!,{r0-r3,r12,pc}^		// restore registers and return (^ = restore CPSR from SPSR_irq)

.ltorg

// ----------------------------------------------------------------------------
//                          L1 flush data cache
// ----------------------------------------------------------------------------

#if RASPPI > 1

/*
 * cache-v7.S
 *
 *  was: linux/arch/arm/mm/cache-v7.S
 *
 *  Copyright (C) 2001 Deep Blue Solutions Ltd.
 *  Copyright (C) 2005 ARM Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  This is the "shell" of the ARMv7 processor support.
 *
 * Modified for Circle by R. Stange
 */

/*
 * The secondary kernel init calls v7_flush_dcache_all before it enables
 * the L1; however, the L1 comes out of reset in an undefined state, so
 * the clean + invalidate performed by v7_flush_dcache_all causes a bunch
 * of cache lines with uninitialized data and uninitialized tags to get
 * written out to memory, which does really unpleasant things to the main
 * processor.  We fix this by performing an invalidate, rather than a
 * clean + invalidate, before jumping into the kernel.
 *
 * This function is cloned from arch/arm/mach-tegra/headsmp.S, and needs
 * to be called for both secondary cores startup and primary core resume
 * procedures.
 */

	.section .text.InvalidateDataCacheL1Only
	.align	2
.global  InvalidateDataCacheL1Only
InvalidateDataCacheL1Only:

	// push registers R4..R6
	push	{r4-r6}

	// select L1 Data Cache (CSSELR = Cache Size Selection Register)
	mov	r0,#0
	mcr	p15,2,r0,c0,c0,0		// CSSELR = 0 (select L1 D-cache)

	// get L1 data cache info from CCSIDR register-> R0
	//  bit 0..2: LineSize (SetShift = log2(line_size) + 2)
	//  bit 3..12: NumWays - 1
	//  bit 13..31: NumSets - 1
	mrc	p15,1,r0,c0,c0,0		// CCSIDR -> R0 (data cache info)

	// prepare number of sets -> R2
	movw	r1,#0x7fff			// R1 <- mask of 15 valid bits of the value
	and	r2,r1,r0,lsr #13		// R2 <- NumSets - 1 (shift and mask value from CCSIDR)
	add	r2,r2,#1			// R2 <- NumSets

	// prepare number of ways -> R4
	movw	r1,#0x3ff			// R1 <- mask of 10 valid bits of the value
	and	r3,r1,r0,lsr #3			// R3 <- NumWays - 1 (shift and mask value from CCSIDR)
	add	r4,r3,#1			// R4 <- NumWays

	// prepare shift of the Set index -> R0
	and	r0,r0,#0x7			// R0 <- mask LineSize
	add	r0,r0,#4			// R0 <- SetShift

	// prepare shift of the Way index -> R1
	clz	r1,r3				// R1 <- WayShift (way position in the index)

	// Loop, for each Set:
1:	sub	r2,r2,#1			// NumSets--, Set counter
	mov	r3,r4				// R3 Temp <- NumWays

	// Loop, for each Way:
2:	subs	r3,r3,#1			// Temp--, Way counter

	// Index prepare -> R5 (way index -> R5, set index -> R6)
	mov	r5,r3,lsl r1			// way index << WayShift
	mov	r6,r2,lsl r0			// set index << SetShift
	orr	r5,r5,r6			// combine R5 <- (Temp << WayShift) | (NumSets << SetShift)

	// Invalidate data cache by Set/Way
	mcr	p15,0,r5,c7,c6,2		// DCISW: invalidate by set/way

	// continue loop
	bgt	2b				// next way
	cmp	r2,#0
	bgt	1b				// next set

	// Barriers
	dsb	st				// Data Synchronization Barrier
	isb					// Instruction Synchronization Barrier

	// pop registers R4..R6 and return
	pop	{r4-r6}
	bx	lr

.ltorg

#endif // RASPPI > 1

// ----------------------------------------------------------------------------
//                          L1 flush data cache
// ----------------------------------------------------------------------------
/*
 *	was: v7_flush_dcache_all()
 *
 *	Invalidate the whole D-cache.
 *
 *	Corrupted registers: r0-r5, r7, r9-r11
 */

#if RASPPI > 1

	.section .text.InvalidateDataCache
	.align	2
.global	InvalidateDataCache
InvalidateDataCache:

	// push registers
	push	{r4-r5,r7,r9-r11}

	// data memory barrier
	dmb					// ensure ordering with previous memory accesses

	// check cache level
	mrc	p15,1,r0,c0,c0,1		// R0 <- get CLIDR register (cache level ID register)
	mov	r3,r0,lsr #23			// R3 <- get LoC (Level of Coherency)
	ands	r3,r3,#7 << 1			// R3 <- get LoC*2 (number of levels * 2)
	beq	5f				// if LoC is 0, then no need to clean

	// loop over cache levels
	mov	r10,#0				// R10 <- start clean at cache level 0

	// check cache type of this level
1:	add	r2,r10,r10,lsr #1		// R2 <- 3 * current cache level
	mov	r1,r0,lsr r2			// R1 <- extract cache type bits from clidr, get cache type for this level
	and	r1,r1,#7			// R1 <- type of this level of the cache
	cmp	r1,#2				// check D-cache type (0=none, 1=I-only, 2=D-only, 3=unified)
	blt	4f				// skip if no cache, or just i-cache

	// select current cache level, and get CSIDR
	mcr	p15,2,r10,c0,c0,0		// select current cache level in CSSR
	isb					// isb to sych the new CSSR & CSIDR
	mrc	p15,1,r1,c0,c0,0		// R1 <- read the new CSIDR

	// get length of cache lines -> R2
	and	r2,r1,#7			// R2 <- extract the length of the cache lines
	add	r2,r2,#4			// R2 <- add 4 (line length offset)

	// get bit position of way size increment -> R4, R5
	movw	r4,#0x3ff			// R4 <- mask
	ands	r4,r4,r1,lsr #3			// R4 <- find maximum number on the way size
	clz	r5,r4				// R5 <- find bit position of way size increment

	// get max number of index size -> R7
	movw	r7,#0x7fff			// R7 <- mask
	ands	r7,r7,r1,lsr #13		// R7 <- extract max number of the index size (bits 13..27)

	// 2 loops - through way and through index
2:	mov	r9,r7				// create working copy of max index
3:	orr	r11,r10,r4,lsl r5		// R11 <- factor way and cache number into r11
	orr	r11,r11,r9,lsl r2		// R11 <- factor index number into r11
	mcr	p15,0,r11,c7,c6,2		// invalidate DCISW by set/way
	subs	r9,r9,#1			// decrement the index
	bge	3b				// next index
	subs	r4,r4,#1			// R4 <- decrement the way
	bge	2b				// next way

	// next cache number
4:	add	r10,r10,#2			// R10 <- increment cache number
	cmp	r3,r10				// check max. cache number
	bgt	1b				// next cache number

	// restore cache level
5:	mov	r10,#0				// swith back to cache level 0
	mcr	p15,2,r10,c0,c0,0		// select current cache level in CSSR

	// barriers
	dsb	st				// data synchronization barrier
	isb					// instruction synchronization barrier

	// pop registers
	pop	{r4-r5,r7,r9-r11}
	bx	lr

.ltorg

#endif // RASPPI > 1

// ----------------------------------------------------------------------------
//                          L1 clean data cache
// ----------------------------------------------------------------------------
/*
 *	Clean the whole D-cache.
 *
 *	Corrupted registers: r0-r5, r7, r9-r11
 */

#if RASPPI > 1

	.section .text.CleanDataCache
	.align	2
.global	CleanDataCache
CleanDataCache:

	// push registers
	push	{r4-r5, r7, r9-r11}

	// data memory barrier
	dmb					// ensure ordering with previous memory accesses

	// check cache level
	mrc	p15,1,r0,c0,c0,1		// R0 <- get CLIDR register (cache level ID register)
	mov	r3,r0,lsr #23			// R3 <- get LoC (Level of Coherency)
	ands	r3,r3,#7 << 1			// R3 <- get LoC*2 (number of levels * 2)
	beq	5f				// if LoC is 0, then no need to clean

	// loop over cache levels
	mov	r10,#0				// R10 <- start clean at cache level 0

	// check cache type of this level
1:	add	r2,r10,r10,lsr #1		// R2 <- 3 * current cache level
	mov	r1,r0,lsr r2			// R1 <- extract cache type bits from clidr, get cache type for this level
	and	r1,r1,#7			// R1 <- type of this level of the cache
	cmp	r1,#2				// check D-cache type (0=none, 1=I-only, 2=D-only, 3=unified)
	blt	4f				// skip if no cache, or just i-cache

	// select current cache level, and get CSIDR
	mcr	p15,2,r10,c0,c0,0		// select current cache level in CSSR
	isb					// isb to sych the new CSSR & CSIDR
	mrc	p15,1,r1,c0,c0,0		// R1 <- read the new csidr

	// get length of cache lines -> R2
	and	r2,r1,#7			// extract the length of the cache lines
	add	r2,r2,#4			// add 4 (line length offset)

	// get bit position of way size increment -> R4, R5
	movw	r4,#0x3ff			// R4 <- mask
	ands	r4,r4,r1,lsr #3			// R4 <- find maximum number on the way size
	clz	r5,r4				// R5 <- find bit position of way size increment

	// get max number of index size -> R7
	movw	r7,#0x7fff			// R7 <- mask
	ands	r7,r7,r1,lsr #13		// R7 <- extract max number of the index size (bits 13..27)

	// 2 loops - through way and through index
2:	mov	r9,r7				// create working copy of max index
3:	orr	r11,r10,r4,lsl r5		// R11 <- factor way and cache number into r11
	orr	r11,r11,r9,lsl r2		// R11 <- factor index number into r11
	mcr	p15,0,r11,c7,c10,2		// clean DCISW by set/way
	subs	r9,r9,#1			// decrement the index
	bge	3b				// next index
	subs	r4,r4,#1			// decrement the way
	bge	2b				// next way

	// next cache number
4:	add	r10,r10,#2			// R10 <- increment cache number
	cmp	r3,r10				// check max. cache number
	bgt	1b				// next cache number

	// restore cache level
5:	mov	r10,#0				// swith back to cache level 0
	mcr	p15,2,r10,c0,c0,0		// select current cache level in CSSR

	// barriers
	dsb	st				// data synchronization barrier
	isb					// instruction synchronization barrier

	// pop registers
	pop	{r4-r5, r7, r9-r11}
	bx	lr

.ltorg

#endif // RASPPI > 1

// ----------------------------------------------------------------------------
//                          Clean data cache in range
// ----------------------------------------------------------------------------
/*
 *	was: v7_flush_kern_dcache_area(void *addr, size_t size)
 *
 *	Ensure that the data held in the page kaddr is written back
 *	to the page in question.
 *
 *	- addr	- kernel address
 *	- size	- region size
 */

#if RASPPI > 1

/*
 * From: linux/arch/arm/mm/proc-macros.S
 *
 * dcache_line_size - get the minimum D-cache line size from the CTR register
 * on ARMv7.
 */
	.macro	dcache_line_size, reg, tmp
	mrc	p15,0,\tmp,c0,c0,1		// read ctr
	lsr	\tmp,\tmp,#16
	and	\tmp,\tmp,#0xf			// cache line size encoding
	mov	\reg,#4				// bytes per word
	mov	\reg,\reg,lsl \tmp		// actual cache line size
	.endm

	.section .text.CleanAndInvalidateDataCacheRange
	.align	2
.global	CleanAndInvalidateDataCacheRange
CleanAndInvalidateDataCacheRange:
	dcache_line_size r2,r3
	add	r1,r0,r1
	sub	r3,r2,#1
	bic	r0,r0,r3
	dsb
1:	mcr	p15,0,r0,c7,c14,1		// clean & invalidate D line / unified line
	add	r0,r0,r2
	cmp	r0,r1
	blo	1b
	dsb	st
	bx	lr

.ltorg

#endif // RASPPI > 1

// ----------------------------------------------------------------------------
//                       Initialize VFP coprocessor
// ----------------------------------------------------------------------------
// Enable VFP/NEON (CP10/CP11), enable VFP unit, set FPSCR
// Safe for ARMv7 (Pi 2/3/Zero2) and ARMv8 in AArch32 mode (Pi 4/5)
// ----------------------------------------------------------------------------

	.section .text.VfpInit
	.align	2
.global VfpInit
VfpInit:
	// enable single and double precision (CPACR = Coprocessor Access Control Register)
	mrc	p15,#0,r1,c1,c0,#2	// read CPACR into R1
	orr	r1,#((3<<20)|(3<<22))	// enable single precision (CP10, 3<<20), enable double precision (CP11, 3<<22)
	mcr	p15,#0,r1,c1,c0,#2	// write CPACR from R1

	// flush prefetch buffer, Instruction Synchronization Barrier (because of FMXR instruction)
#if RASPPI == 1
	mov	r0,#0			// R0 <- 0
	mcr	p15,#0,r0,c7,c5,#4	// instruction sychronization barrier ISB (flush prefetch buffer)
#else
	isb				// instruction sychronization barrier ISB (flush prefetch buffer)
#endif

	// enable VFP unit (FPEXC.EN)
	mov	r0,#(1<<30)
	fmxr	fpexc,r0		// vmsr fpexc,r0

	// Set FPSCR: enable FZ (flush-to-zero, 1<<24), enable DN (default NaN, 1<<25)
	mov	r0,#(1<<25)|(1<<24)	// enable FZ and DN
	fmxr	fpscr,r0		// vmsr fpscr,r0
	bx	lr

.ltorg

// ----------------------------------------------------------------------------
//                       Terminate VFP coprocessor
// ----------------------------------------------------------------------------

	.section .text.VfpTerm
	.align	2
.global VfpTerm
VfpTerm:

	// check if VFP is enabled (or access to FPEXC can chrash)
	mrc	p15,0,r1,c1,c0,#2	// get CPACR
	tst	r1,#(3 << 20)		// is CP10 enabled?
	beq	2f			// CP10 not enabled

	// disable VFP unit (FPEXC.EN)
	vmrs	r0,fpexc
	bic	r0,r0,#(1 << 30)	// disable VFP unit
	vmsr	fpexc,r0

	// flush prefetch buffer, Instruction Synchronization Barrier
#if RASPPI == 1
	mov	r0,#0			// R0 <- 0
	mcr	p15,#0,r0,c7,c5,#4	// instruction sychronization barrier ISB (flush prefetch buffer)
#else
	isb				// instruction sychronization barrier ISB (flush prefetch buffer)
#endif

	// disable single and double precision (CPACR = Coprocessor Access Control Register)
	mrc	p15,#0,r1,c1,c0,#2	// read CPACR into R1
	bic	r1,#((3<<20)|(3<<22))	// disable single precision (CP10, 3<<20), disable double precision (CP11, 3<<22)
	mcr	p15,#0,r1,c1,c0,#2	// write CPACR from R1

	// flush prefetch buffer, Instruction Synchronization Barrier
#if RASPPI == 1
	mov	r0,#0			// R0 <- 0
	mcr	p15,#0,r0,c7,c5,#4	// instruction sychronization barrier ISB (flush prefetch buffer)
#else
	isb				// instruction sychronization barrier ISB (flush prefetch buffer)
#endif

2:	bx	lr

.ltorg

// ----------------------------------------------------------------------------
//                              Enable MMU
// ----------------------------------------------------------------------------

	.section .text.EnableMMU
	.align	2
.global EnableMMU
EnableMMU:

	// push registers
	push	{lr}

#if RASPPI <= 3

	// setup AuxControl
	mrc	p15,0,r1,c1,c0,1	// R1 <- get AuxControl
	orr	r1,r1,#(1 << 6)		// <= Pi 1: restrict cache size to 16K (no page coloring)
					// >= Pi 2: switch on SMP
	mcr	p15,0,r1,c1,c0,1	// set AucControl

	// set TTB control - whole memory with TTBR0
	mov	r1,#0
	mcr	p15,0,r1,c2,c0,2	// set TTBCR_SPLIT = 0

	// set address of page table to TTBR0
	ldr	r1,=PageTableLevel1	// R1 <- address of page table level 1
#if RASPPI == 1
	orr	r1,r1,#(1<<0)		// add attribute INNER CACHEABLE (outer non cacheable)
#else
	orr	r1,r1,#((1<<0)|(3<<3)|(1<<6)) // add attributes INNER/OUTER WRITE BACK
#endif
	mcr	p15,0,r1,c2,c0, 0	// set address of page table

#else	// RASPPI <= 3

	// setup MAIR0
	mov	r0,#0x4FF		// AttrIndx=0 normal WBWA = 0xFF (write back, write allocate)
					// AttrIndx=1 device nGnRE = 0x04 << 8, 
					// AttrIndx=2 coherent = 0 << 16
	mcr	p15,0,r0,c10,c2,0	// setup MAIR0

	// setup TTBCR
	ldr	r0,=0x80803500		// 1 << 31 LPAE enable
					// 1 << 23 EPD1 disabled
					// 3 << 12 inner shareable
					// 1 << 10 WBWA outer write back, write allocate
					// 1 << 8 WBWA inner write back, write allocate
					// 0 4GB VA
	mcr	p15,0,r0,c2,c0,2	// set TTBCR

	// setup TTBR0 - page table address
	ldr	r0,=PageTableLevel1	// R0 <- address of page table level 1 LOW
	mov	r1,#0			// R1 <- address HIGH = 0
	mcrr	p15,0,r0,r1,c2		// set TTBR0

#endif 	// RASPPI <= 3

	// set Domain Access Control register (Domain 0 to client)
	mov	r0,#1			// 1 = client
	mcr	p15,0,r0,c3,c0,0	// set DACR register

	// invalidate data cache
#if RASPPI == 1
	mov	r0,#0
	mcr	p15,0,r0,c7,c6,0	// Invalidate entire data cache
	mcr	p15,0,r0,c7,c10,4	// Drain write buffer (Data Synchronization)
#else
	bl	InvalidateDataCacheL1Only
#endif

	// invalidate instruction cache
	mov	r0,#0
	mcr	p15,0,r0,c7,c5,0	// invalidate instruction cache
	mcr	p15,0,r0,c7,c5,6	// flush branch target cache

	// invalidate unified TLB
	// - required if MMU was previously enabled and not properly reset
	mcr	p15,0,r0,c8,c7, 0	// invalidate unified TLB (R0 = 0 at this point)

	// barrier, flush prefetch buffer
#if RASPPI == 1
	mcr	p15,0,r0,c7,c10,4	// data synchronization barrier (R0 = 0 at this point)
	mcr	p15,0,r0,c7,c5,4	// instruction synchronization barrier (R0 = 0 at this point)
#else
	dsb	sy			// data synchronization barrier
	isb				// instruction synchronization barrier
#endif

	// get current SCTLR control register -> R0
	mrc	p15,0,r0,c1,c0,0	// R0 <- SCTLR control register

	// disable strict alignment
	bic	r0,r0,#(1 << 1)		// disable strict alignment

	// setup MMU mode
	//  M (bit 0): MMU enable
	//  A (bit 1): strict alignment
	//  C (bit 2): L1 D-cache enable
	//  Z (bit 11): branch prediction
	//  I (bit 12): L1 I-cache enable
	//  U (bit 22): 1=unaligned data access enabled
	//  X (bit 23): 1MB sections, extended page table
#if RASPPI == 1
	ldr	r1,=(1<<0)|(1<<2)|(1<<11)|(1<<12)|(1<<22)|(1<<23)
#else
	ldr	r1,=(1<<0)|(1<<2)|(1<<11)|(1<<12)|(1<<22)
#endif
	orr	r0,r0,r1

	// write SCTLR register
	mcr	p15,0,r0,c1,c0,0	// write SCTLR

	// barrier, flush prefetch buffer
#if RASPPI == 1
	mov	r0,#0
	mcr	p15,0,r0,c7,c10,4	// data synchronization barrier (R0 = 0 at this point)
	mcr	p15,0,r0,c7,c5,4	// instruction synchronization barrier (R0 = 0 at this point)
#else
	dsb	sy			// data synchronization barrier
	isb				// instruction synchronization barrier
#endif

	pop	{pc}

.ltorg

// ----------------------------------------------------------------------------
//                              Disable MMU
// ----------------------------------------------------------------------------

	.section .text.DisableMMU
	.align	2
.global DisableMMU
DisableMMU:

	// barrier, flush prefetch buffer
#if RASPPI == 1
	mcr	p15,0,r0,c7,c10,4	// data synchronization barrier (R0 = 0 at this point)
	mcr	p15,0,r0,c7,c5,4	// instruction synchronization barrier (R0 = 0 at this point)
#else
	dsb	sy			// data synchronization barrier
	isb				// instruction synchronization barrier
#endif

	// get current SCTLR control register -> R0
	mrc	p15,0,r0,c1,c0,0	// R0 <- SCTLR control register

	// disable L1 D-cache, disable MMU
	bic	r0,r0,#(1 << 0)		// disable MMU
	bic	r0,r0,#(1 << 2)		// disable L1 D-cache

	// write SCTLR register
	mcr	p15,0,r0,c1,c0,0	// write SCTLR

	// barrier, flush prefetch buffer
#if RASPPI == 1
	mov	r0,#0
	mcr	p15,0,r0,c7,c10,4	// data synchronization barrier (R0 = 0 at this point)
	mcr	p15,0,r0,c7,c5,4	// instruction synchronization barrier (R0 = 0 at this point)
#else
	dsb	sy			// data synchronization barrier
	isb				// instruction synchronization barrier
#endif

	// invalidate unified TLB
	mov	r0,#0			// R0 <- 0
	mcr	p15,0,r0,c8,c7, 0	// invalidate unified TLB

	// invalidate instruction cache and branch predictor
	mcr	p15,0,r0,c7,c5,0	// Invalidate I-Cache
	mcr	p15,0,r0,c7,c5,6	// Invalidate Branch Target Cache (BTB)

	// barrier, flush prefetch buffer
#if RASPPI == 1
	mov	r0,#0
	mcr	p15,0,r0,c7,c10,4	// data synchronization barrier (R0 = 0 at this point)
	mcr	p15,0,r0,c7,c5,4	// instruction synchronization barrier (R0 = 0 at this point)
#else
	dsb	sy			// data synchronization barrier
	isb				// instruction synchronization barrier
#endif

	bx	lr

.ltorg

// ----------------------------------------------------------------------------
//                    Trampoline, to start loaded program
// ----------------------------------------------------------------------------
//  R0 = source address
//  R1 = data length in bytes (> 0)
//  R2 = destination address

	.section .text.Trampoline
	.align	2
.global Trampoline
Trampoline:

	// save destination address -> R3
	mov	r3,r2			// R3 <- destination address

	// convert to number of words
	add	r1,r1,#3		// R1 <- round length up
	lsrs	r1,r1,#2		// R1 <- (len + 3) / 4

	// copy loop
2:	ldr	r4,[r0],#4		// R4 <- load 32-bit word, shift pointer
	str	r4,[r2],#4		// store 32-bit word, shift pointer
	subs	r1,r1,#1		// loop counter
	bne	2b			// next word

#if RASPPI > 1
	// wait for transfer end
	dsb	sy			// wait for transfer end
#endif

	// invalidate instruction cache
	mov	r0,#0
	mcr	p15,0,r0,c7,c5,0	// invalidate entire instruction cache
	mcr	p15,0,r0,c7,c5,6	// invalidate branch target cache
#if RASPPI == 1
	mcr	p15,0,r0,c7,c5,4	// instruction synchronization barrier
#else
	dsb	sy			// wait for transfer end
	isb				// instruction synchronization barrier
#endif

	// jump to new code
	bx	r3

.ltorg

.global Trampoline_End
Trampoline_End:

// ----------------------------------------------------------------------------
//                         Wrap memset function
// ----------------------------------------------------------------------------
// The memset() library function uses unaligned memory access. This is a problem
// when accessing uncached memory - in which case the library function crashes.
// The following function provides aligned access.
// INPUT:
//  R0 = destination address
//  R1 = value
//  R2 = length in bytes
// OUTPUT:
//  R0 = original destination address

	.section .text.__wrap_memset
	.align	2
.global __wrap_memset
__wrap_memset:

	// destination address -> R3 (we must return original destination address in R0)
	mov	r3,r0		// R3 <- destination address

	// check small memory block
	cmp	r2,#16		// check if size is at least 16 bytes
	blt	5f		// small block - skip large transfers

	// store start of data by bytes
2:	tst	r3,#3		// check if address is aligned to 4 bytes
	beq	3f		// skip, address is already aligned
	strb	r1,[r3],#1	// store byte and increase address
	subs	r2,r2,#1	// loop counter
	bne	2b		// store next byte
	bx	lr		// all done
	
	// prepare data to store
3:	and	r1,r1,#0xff	// R1 <- mask byte to store
	orr	r1,r1,r1,lsl #8	// R1 <- duplicate to 2 bytes
	orr	r1,r1,r1,lsl #16 // R1 <- duplicate to 4 bytes

	// store aligned data by 2 words
4:	cmp	r2,#8		// check if size is at least 8 bytes
	blt	5f		// insufficient data
	str	r1,[r3],#4	// store word and increase address
	str	r1,[r3],#4	// store word and increase address
	subs	r2,r2,#8	// loop counter
	bgt	4b		// next data

	// store rest of data by bytes
5:	cmp	r2,#0		// check if size is 0
	beq	9f		// exit if size is 0
6:	strb	r1,[r3],#1	// store byte and increase address
	subs	r2,r2,#1	// loop counter
	bne	6b		// next byte

9:	bx	lr

.ltorg

// ----------------------------------------------------------------------------
//                         Wrap memcpy function
// ----------------------------------------------------------------------------
// The memcpy() library function uses unaligned memory access. This is a problem
// when accessing uncached memory - in which case the library function crashes.
// The following function provides aligned access.
// INPUT:
//  R0 = destination address
//  R1 = source address
//  R2 = length in bytes
// OUTPUT:
//  R0 = original destination address

	.section .text.__wrap_memcpy
	.align	2
.global __wrap_memcpy
__wrap_memcpy:

	// destination address -> R3 (we must return original destination address in R0)
	mov	r3,r0		// R3 <- destination address

	// check small memory block
	cmp	r2,#16		// check if size is at least 16 bytes
	blt	5f		// small block - skip large transfers

	// check identical alignment to 4 bytes
	eor	ip,r3,r1	// IP <- dst ^ src
	tst	ip,#3		// compare alignment
	bne	5f		// different alignment - whole block must be transfered by bytes

	// copy start of data by bytes
2:	tst	r3,#3		// check if address is aligned to 4 bytes
	beq	4f		// skip, address is already aligned
	ldrb	ip,[r1],#1	// IP <- load byte and increase address
	subs	r2,r2,#1	// loop counter (delay after ldrb)
	strb	ip,[r3],#1	// store byte and increase address
	bne	2b		// copy next byte
	bx	lr		// all done

	// copy aligned data by word
4:	cmp	r2,#4		// check if size is at least 4 bytes
	blt	5f		// insufficient data
	ldr	ip,[r1],#4	// load word and increase address
	subs	r2,r2,#4	// loop counter (delay after ldr)
	str	ip,[r3],#4	// store word and increase address
	bgt	4b		// next data

	// copy rest of data by bytes
5:	cmp	r2,#0		// check if size is 0
	beq	9f		// exit if size is 0
6:	ldrb	ip,[r1],#1	// IP <- load byte and increase address
	subs	r2,r2,#1	// loop counter (delay after ldrb)
	strb	ip,[r3],#1	// store byte and increase address
	bne	6b		// next byte

9:	bx	lr

.ltorg

// ----------------------------------------------------------------------------
//                         Wrap memcmp function
// ----------------------------------------------------------------------------
// The memcmp() library function uses unaligned memory access. This is a problem
// when accessing uncached memory - in which case the library function crashes.
// The following function provides aligned access.
// INPUT:
//  R0 = address 1
//  R1 = address 2
//  R2 = length in bytes
// OUTPUT:
//  R0 = difference

	.section .text.__wrap_memcmp
	.align	2
.global __wrap_memcmp
__wrap_memcmp:

	// no data, memory equal
	cmp	r2,#0
	beq	6f		// skip if no data

	// compare loop
2:	ldrb	r3,[r0],#1	// R3 <- load byte 1 and increment
	ldrb	r12,[r1],#1	// R12 <- load byte 2 and increment
	cmp	r3,r12		// compare bytes
	bne	8f		// difference
	subs	r2,r2,#1	// byte counter
	bne	2b		// next byte

	// memory equal
6:	mov	r0,#0		// result is 0
	bx	lr

	// memory difference
8:	sub	r0,r3,r12	// R0 <- byte difference
	bx	lr

.ltorg

// ----------------------------------------------------------------------------
//                         Wrap memmove function
// ----------------------------------------------------------------------------
// The memmove() library function uses unaligned memory access. This is a problem
// when accessing uncached memory - in which case the library function crashes.
// The following function provides aligned access.
// INPUT:
//  R0 = destination address
//  R1 = source address
//  R2 = length in bytes
// OUTPUT:
//  R0 = original destination address

	.section .text.__wrap_memmove
	.align	2
.global __wrap_memmove
__wrap_memmove:

	// check if size is 0
	cmp	r2,#0		// check if size is 0
	beq	9f		// exit if size is 0

	// destination address -> R3 (we must return original destination address in R0)
	mov	r3,r0		// R3 <- destination address

	// compare adresses
	cmp	r3,r1		// compare dst ? src
	blo	6f		// if dst < src, copy forward (up)
	beq	9f		// if dst == src, exit

	// copy backward
	add	r1,r1,r2	// R1 <- shift source address to the end
	add	r3,r3,r2	// R3 <- shift destination address to the end
4:	ldrb	ip,[r1,#-1]!	// IP <- pre-decrement address and get byte
	subs	r2,r2,#1	// loop counter (delay after ldrb)
	strb	ip,[r3,#-1]!	// pre-decrement address and store byte
	bne	4b
	bx	lr

	// copy forward
6:	ldrb	ip,[r1],#1	// IP <- load byte and increase address
	subs	r2,r2,#1	// loop counter (delay after ldrb)
	strb	ip,[r3],#1	// store byte and increase address
	bne	6b		// next byte
9:	bx	lr

.ltorg
