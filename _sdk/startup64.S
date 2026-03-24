
// ****************************************************************************
//
//                         Startup code, 64-bit mode
//
// ****************************************************************************
// Some parts of the code were taken from the Circle library:
/*
 * startup64.S
 *
 * Circle - A C++ bare metal environment for Raspberry Pi
 * Copyright (C) 2014-2024  R. Stange <rsta2@o2online.de>
 *
 * This file contains (sligthly modified) code taken from U-Boot:
 *	armv8_switch_to_el1_m macro
 *	defined in arch/arm/include/asm/macro.h
 *	Copyright (C) 2009 Jean-Christophe PLAGNIOL-VILLARD <plagnioj@jcrosoft.com>
 *	Licensed under GPL-2.0+
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
// - Registers available to modify without push: X0-X18
// - Registers need to be saved: X19-X29, X30 (LR)

#include "config.h"

	// macro to swich CPU from EL2 mode to EL1t mode
	.macro armv8_switch_to_el1_m, xreg1,xreg2

	// Initialize Generic Timers
	mrs	\xreg1,cnthctl_el2
	orr	\xreg1,\xreg1,#0x3	// Enable EL1 access to timers
	msr	cnthctl_el2,\xreg1
	msr	cntvoff_el2,xzr

	// Initilize MPID/MPIDR registers
	mrs	\xreg1,midr_el1
	mrs	\xreg2,mpidr_el1
	msr	vpidr_el2,\xreg1
	msr	vmpidr_el2,\xreg2

	// Disable coprocessor traps
	mov	\xreg1,#0x33ff
	msr	cptr_el2,\xreg1		// Disable coprocessor traps to EL2
	msr	hstr_el2,xzr		// Disable coprocessor traps to EL2
	mov	\xreg1,#3 << 20
	msr	cpacr_el1,\xreg1	// Enable FP/SIMD at EL1

	// Initialize HCR_EL2
	mov	\xreg1,#(1 << 31)	// 64bit EL1
	msr	hcr_el2,\xreg1

	/* SCTLR_EL1 initialization
	 *
	 * setting RES1 bits (29,28,23,22,20,11) to 1
	 * and RES0 bits (31,30,27,21,17,13,10,6) +
	 * UCI,EE,EOE,WXN,nTWE,nTWI,UCT,DZE,I,UMA,SED,ITD,
	 * CP15BEN,SA0,SA,C,A,M to 0
	 */
	mov	\xreg1,#0x0800
	movk	\xreg1,#0x30d0,lsl #16
	msr	sctlr_el1,\xreg1

	// Return to the EL1_SP1 mode from EL2
	mov	\xreg1,#0x3c4
	msr	spsr_el2,\xreg1		// EL1_SP0 | D | A | I | F
	adr	\xreg1,1f
	msr	elr_el2,\xreg1
	eret
1:
	.endm

// ---------------------------------------------------------------------------
//               Start after reset (boot at address 0x80000)
// ---------------------------------------------------------------------------

	.section .init	// init code section
.global _start
_start:

// ---- clear BSS segment

	ldr	x0,=__bss_start		// X0 <- BSS start address
	ldr	x1,=__bss_end		// X1 <- BSS end address
1:	cmp	x0,x1			// reached end?
	bge	2f			// skip if end reached
	str	xzr,[x0],#8		// store 0, advance X0 pointer by 8 bytes
	b	1b			// loop

// ---- Switch CPU from EL2 mode to EL1t mode

	// check if CPU is already in EL1t mode
2:	mrs	x0,CurrentEL		// check if already in EL1t mode?
	cmp	x0,#4			// 0x4 = EL1, 0x8 = EL2
	beq	4f			// skip if already EL1 mode

	// setup stack for exception handler
	ldr	x0,=__stack_end - SYS_STACK_SIZE // IRQ, FIQ and exception handler run in EL1h
	msr	sp_el1,x0		// init IRQ stack

	// setup vector table for EL2 mode
	ldr	x0,=VectorTable		// init exception vector table for EL2
	msr	vbar_el2,x0

	// switch to EL1
	armv8_switch_to_el1_m x0,x1

// ---- Setup stack pointers

	// setup stack for exception handler
	// - This is needed even in EL1t mode (start from boot loader in EL1t mode)
4:	msr	spsel,#1		// select SP1
	isb
	ldr	x0,=__stack_end - SYS_STACK_SIZE // IRQ, FIQ and exception handler run in EL1h
	mov	sp,x0			// init IRQ stack

	// initialize main stack
	msr	spsel,#0		// select back SP0
	isb
	ldr	x0,=__stack_end		// prepare stack end of last core
	mov	sp,x0			// init main stack

// ---- Setup vector table

	// initialite vector table
	ldr	x0,=VectorTable		// init exception vector table
	msr	vbar_el1,x0

	// jump to system initialize
	b	SysInit

.ltorg

// ---------------------------------------------------------------------------
//                        Start secondary core
// ---------------------------------------------------------------------------

#if USE_MULTICORE		// 1=use multicore (for applications), 0=do not use other cores (for loader)

	.section .init	// init code section
	.align	3
.global StartSecondary
StartSecondary:

	// read CPU ID -> X2
	mrs	x2,mpidr_el1		// read affinity
#if RASPPI >= 5
	lsr	x2,x2,#8		// CPU ID is in Aff1 in Cortex-A76
#endif
	and	x2,x2,#CORES-1		// get CPU ID

	// setup stack for exception handler
	msr	spsel,#1		// select SP1
	isb
	ldr	x1,=CORE_STACK_SIZE	// calculate kernel stack offset for core
	mul	x1,x1,x2
	ldr	x0,=__stack_end - SYS_STACK_SIZE // IRQ, FIQ and exception handler run in EL1h
	sub	x0,x0,x1		// X0 <- stack of this core
	mov	sp,x0			// init IRQ stack
	msr	spsel,#0		// select back SP0
	isb

	// check if CPU is already in EL1t mode
	mrs	x0,CurrentEL		// check if already in EL1t mode?
	cmp	x0,#4			// 0x4 = EL1, 0x8 = EL2
	beq	4f			// skip if already EL1 mode

	// switch to EL1
	armv8_switch_to_el1_m x0,x1

	// initialize stack of current core
4:	ldr	x1,=CORE_STACK_SIZE	// calculate kernel stack offset for core
	mul	x1,x1,x2
	ldr	x0,=__stack_end		// prepare stack end of last core
	sub	x0,x0,x1		// X0 <- stack of this core
	mov	sp,x0			// init its stack

	// initialize vector table
	ldr	x0,=VectorTable		// init exception vector table
	msr	vbar_el1,x0

	// jump to core initialize
	b	SysInitSecondary

.ltorg

#endif // USE_MULTICORE

// ---------------------------------------------------------------------------
//                             Vector table
// ---------------------------------------------------------------------------

	// Macro: vector handler
	.macro	vector handler
	.align	7			// align to 128 bytes
	b	\handler		// jump to handler
	.endm

	.text
	.align	11			// align to 2048 bytes
.global	VectorTable
VectorTable:

	// from current EL with sp_el0
	vector	SynchronousStub
	vector	IRQStub
	vector	FIQStub
	vector	SErrorStub

	// from current EL with sp_elx, x != 0
	vector	SynchronousStub
	vector	IRQStub
	vector	FIQStub
	vector	SErrorStub

	// from lower EL, target EL minus 1 is AArch64
	vector	HVCStub
	vector	UnexpectedStub
	vector	UnexpectedStub
	vector	UnexpectedStub

	// from lower EL, target EL minus 1 is AArch32
	vector	UnexpectedStub
	vector	UnexpectedStub
	vector	UnexpectedStub
	vector	UnexpectedStub

	.align	3
UnexpectedStub:
SynchronousStub:
SErrorStub:
HVCStub:
	b	.

	// IRQ stub
	// ... Every interrupt takes 824 bytes of the RAM, minimal interrupt time about 250ns 

	.align	3
.global	IRQStub
IRQStub:

	// save registers x29 (FP) and x30 (LR)
	stp	x29,x30,[sp,#-16]!		// save x29, x30 onto stack

	// save ELR (exception LR) and SPSR (exception status)
	mrs	x29,elr_el1			// save elr_el1, spsr_el1 onto stack
	mrs	x30,spsr_el1
	stp	x29,x30,[sp,#-16]!

	// save FPU registers
	stp	q30,q31,[sp,#-32]!		// save q0-q31 onto stack
	stp	q28,q29,[sp,#-32]!
	stp	q26,q27,[sp,#-32]!
	stp	q24,q25,[sp,#-32]!
	stp	q22,q23,[sp,#-32]!
	stp	q20,q21,[sp,#-32]!
	stp	q18,q19,[sp,#-32]!
	stp	q16,q17,[sp,#-32]!
	stp	q14,q15,[sp,#-32]!
	stp	q12,q13,[sp,#-32]!
	stp	q10,q11,[sp,#-32]!
	stp	q8,q9,[sp,#-32]!
	stp	q6,q7,[sp,#-32]!
	stp	q4,q5,[sp,#-32]!
	stp	q2,q3,[sp,#-32]!
	stp	q0,q1,[sp,#-32]!

	// save GPR registers
	stp	x27,x28,[sp,#-16]!		// save x0-x28 onto stack
	stp	x25,x26,[sp,#-16]!
	stp	x23,x24,[sp,#-16]!
	stp	x21,x22,[sp,#-16]!
	stp	x19,x20,[sp,#-16]!
	stp	x17,x18,[sp,#-16]!
	stp	x15,x16,[sp,#-16]!
	stp	x13,x14,[sp,#-16]!
	stp	x11,x12,[sp,#-16]!
	stp	x9,x10,[sp,#-16]!
	stp	x7,x8,[sp,#-16]!
	stp	x5,x6,[sp,#-16]!
	stp	x3,x4,[sp,#-16]!
	stp	x1,x2,[sp,#-16]!
	str	x0,[sp,#-16]!

	// call interrupt handler
	bl	IRQ_Dispatch

	// restore GPR registers
	ldr	x0,[sp],#16			// restore x0-x28 from stack
	ldp	x1,x2,[sp],#16
	ldp	x3,x4,[sp],#16
	ldp	x5,x6,[sp],#16
	ldp	x7,x8,[sp],#16
	ldp	x9,x10,[sp],#16
	ldp	x11,x12,[sp],#16
	ldp	x13,x14,[sp],#16
	ldp	x15,x16,[sp],#16
	ldp	x17,x18,[sp],#16
	ldp	x19,x20,[sp],#16
	ldp	x21,x22,[sp],#16
	ldp	x23,x24,[sp],#16
	ldp	x25,x26,[sp],#16
	ldp	x27,x28,[sp],#16

	// restore FPU registers
	ldp	q0,q1,[sp],#32			// restore q0-q31 from stack
	ldp	q2,q3,[sp],#32
	ldp	q4,q5,[sp],#32
	ldp	q6,q7,[sp],#32
	ldp	q8,q9,[sp],#32
	ldp	q10,q11,[sp],#32
	ldp	q12,q13,[sp],#32
	ldp	q14,q15,[sp],#32
	ldp	q16,q17,[sp],#32
	ldp	q18,q19,[sp],#32
	ldp	q20,q21,[sp],#32
	ldp	q22,q23,[sp],#32
	ldp	q24,q25,[sp],#32
	ldp	q26,q27,[sp],#32
	ldp	q28,q29,[sp],#32
	ldp	q30,q31,[sp],#32

	// restore ELR (exception LR) and SPSR (exception status)
	ldp	x29,x30,[sp],#16		// restore elr_el1, spsr_el1 from stack
	msr	elr_el1,x29
	msr	spsr_el1,x30

	// restore registers x29 (FP) and x30 (LR)
	ldp	x29,x30,[sp],#16		// restore x29, x30 from stack
	eret

.ltorg

	// FIQ stub
	.align	3
.global	FIQStub
FIQStub:

#if FIQ_SAVE_VFP
	// save FPU registers
	stp	q30,q31,[sp,#-32]!
	stp	q28,q29,[sp,#-32]!
	stp	q26,q27,[sp,#-32]!
	stp	q24,q25,[sp,#-32]!
	stp	q22,q23,[sp,#-32]!
	stp	q20,q21,[sp,#-32]!
	stp	q18,q19,[sp,#-32]!
	stp	q16,q17,[sp,#-32]!
	stp	q14,q15,[sp,#-32]!
	stp	q12,q13,[sp,#-32]!
	stp	q10,q11,[sp,#-32]!
	stp	q8,q9,[sp,#-32]!
	stp	q6,q7,[sp,#-32]!
	stp	q4,q5,[sp,#-32]!
	stp	q2,q3,[sp,#-32]!
	stp	q0,q1,[sp,#-32]!
#endif // FIQ_SAVE_VFP

	// save GPR registers
	stp	x29,x30,[sp,#-16]!
	stp	x27,x28,[sp,#-16]!
	stp	x25,x26,[sp,#-16]!
	stp	x23,x24,[sp,#-16]!
	stp	x21,x22,[sp,#-16]!
	stp	x19,x20,[sp,#-16]!
	stp	x17,x18,[sp,#-16]!
	stp	x15,x16,[sp,#-16]!
	stp	x13,x14,[sp,#-16]!
	stp	x11,x12,[sp,#-16]!
	stp	x9,x10,[sp,#-16]!
	stp	x7,x8,[sp,#-16]!
	stp	x5,x6,[sp,#-16]!
	stp	x3,x4,[sp,#-16]!
	stp	x1,x2,[sp,#-16]!
	str	x0,[sp,#-16]!

	// call FIQ handler
	ldr	x2,=FIQHandlerAddr
	ldr	x1,[x2]				// get FIQData.pHandler
	blr	x1				// call handler

	// restore GPR registers
1:	ldr	x0,[sp],#16
	ldp	x1,x2,[sp],#16
	ldp	x3,x4,[sp],#16
	ldp	x5,x6,[sp],#16
	ldp	x7,x8,[sp],#16
	ldp	x9,x10,[sp],#16
	ldp	x11,x12,[sp],#16
	ldp	x13,x14,[sp],#16
	ldp	x15,x16,[sp],#16
	ldp	x17,x18,[sp],#16
	ldp	x19,x20,[sp],#16
	ldp	x21,x22,[sp],#16
	ldp	x23,x24,[sp],#16
	ldp	x25,x26,[sp],#16
	ldp	x27,x28,[sp],#16
	ldp	x29,x30,[sp],#16

#if FIQ_SAVE_VFP
	// restore FPU registers
	ldp	q0,q1,[sp],#32
	ldp	q2,q3,[sp],#32
	ldp	q4,q5,[sp],#32
	ldp	q6,q7,[sp],#32
	ldp	q8,q9,[sp],#32
	ldp	q10,q11,[sp],#32
	ldp	q12,q13,[sp],#32
	ldp	q14,q15,[sp],#32
	ldp	q16,q17,[sp],#32
	ldp	q18,q19,[sp],#32
	ldp	q20,q21,[sp],#32
	ldp	q22,q23,[sp],#32
	ldp	q24,q25,[sp],#32
	ldp	q26,q27,[sp],#32
	ldp	q28,q29,[sp],#32
	ldp	q30,q31,[sp],#32
#endif // FIQ_SAVE_VFP

	eret

.ltorg

// ----------------------------------------------------------------------------
//                              Enable MMU
// ----------------------------------------------------------------------------

	.section .text.EnableMMU
	.align	3
.global EnableMMU
EnableMMU:

	// setup MAIR_EL1
	mov	x0,#0x4FF		// AttrIndx=0 normal WBWA = 0xFF (write back, write allocate)
					// AttrIndx=1 device nGnRE = 0x04 << 8, 
					// AttrIndx=2 coherent = 0 << 16
	msr	mair_el1,x0		// setup MAIR_EL1

	// setup TCR_EL1
	// 0 << 32 ... IPS = 0, address space is 4 GB
	// 1 << 23 ... EPD = 1, TTBR1 is off
	// 0 << 14 ... TG0 = 0, 4 KB granularity (1 = 64 KB, 2 = 16 KB)
	// 3 << 12 ... SH0 = 3, inner shareable
	// 1 << 10 ... ORGN0 = 1, outer write back allocate
	// 1 << 8 .... IRGN0 = 1, inner write back allocate
	// 32 << 0 ... T0SZ = 32, virtual address space is 4 GB
	ldr	x0,=((0ull << 32) | \
			(1 << 23) | \
			(0 << 14) | \
			(3 << 12) | \
			(1 << 10) | \
			(1 << 8) | \
			(32 << 0))
	msr	tcr_el1,x0		// set TCR_EL1
	isb

	// setup TTBR0_EL1 - L1 page table address
	ldr	x0,=PageTableLevel1	// X0 <- address of page table level 1
	msr	ttbr0_el1,x0		// set TTBR0_EL1

	// barrier, flush prefetch buffer
	dsb	sy			// data synchronization barrier
	isb				// instruction synchronization barrier

	// setup SCTLR_EL1 control register
	mrs	x0,sctlr_el1		// X0 <- get SCTLR_EL1 control register
	orr	x0,x0,#(1 << 0)		// M – MMU enable
	bic	x0,x0,#(1 << 1)		// A - strict alignment disable
	orr	x0,x0,#(1 << 2)		// C – D-cache enable
	orr	x0,x0,#(1 << 12)	// I – I-cache enable
	bic	x0,x0,#(1 << 19)	// WXN - clear WXN, no W+X protection
	orr	x0,x0,#(1 << 22)	// U - unaligned data access enabled
	msr	sctlr_el1,x0		// write SCTLR_EL1

	// barrier, flush prefetch buffer
	dsb	sy			// data synchronization barrier
	isb				// instruction synchronization barrier
	ret

.ltorg

// ----------------------------------------------------------------------------
//                              Disable MMU
// ----------------------------------------------------------------------------

	.section .text.DisableMMU
	.align	3
.global DisableMMU
DisableMMU:

	// invalidate TLB_EL1
	tlbi	vmalle1			// invalidate TLB

	// barrier, flush prefetch buffer
	dsb	sy			// data synchronization barrier
	isb				// instruction synchronization barrier

	// setup SCTLR_EL1 control register - disable MMU and D-cache
	mrs	x0,sctlr_el1		// X0 <- get SCTLR_EL1 control register
	bic	x0,x0,#(1 << 0)		// MMU disable
	bic	x0,x0,#(1 << 2)		// D-cache disable
	msr	sctlr_el1,x0		// write SCTLR_EL1

	// barrier, flush prefetch buffer
	dsb	sy			// data synchronization barrier
	isb				// instruction synchronization barrier
	ret

.ltorg

// ----------------------------------------------------------------------------
//                    Trampoline, to start loaded program
// ----------------------------------------------------------------------------
//  X0 = source address
//  X1 = data length in bytes (> 0)
//  X2 = destination address

	.section .text.Trampoline
	.align	3
.global Trampoline
Trampoline:

	// save destination address -> X3
	mov	x3,x2			// X3 <- destination address

	// convert to number of double-words
	add	x1,x1,#7		// X1 <- round length up
	lsr	x1,x1,#3		// X1 <- (len + 7) / 8

	// copy loop
2:	ldr	x4,[x0],#8		// X4 <- load 64-bit word, shift pointer
	str	x4,[x2],#8		// store 64-bit word, shift pointer
	subs	x1,x1,#1		// loop counter
	bne	2b			// next word

	// invalidate instruction cache
	ic	iallu			// invalidate all instruction caches
	dsb	ish
	isb				// instruction synchronization barrier

	// emergency setup stack for exception handler - for case if app do not setup its own IRQ stack
	msr	spsel,#1		// select SP1
	isb
	mov	sp,x3			// init IRQ stack
	msr	spsel,#0		// select back SP0
	isb

	// jump to new code
	br	x3

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
//  X0 = destination address
//  X1 = value
//  X2 = length in bytes
// OUTPUT:
//  X0 = original destination address

	.section .text.__wrap_memset
	.align	3
.global __wrap_memset
__wrap_memset:

	// destination address -> X3 (we must return original destination address in X0)
	mov	x3,x0		// X3 <- destination address

	// check small memory block
	cmp	x2,#32		// check if size is at least 32 bytes
	blt	5f		// small block - skip large transfers

	// store start of data by bytes
2:	tst	x3,#7		// check if address is aligned to 8 bytes
	beq	3f		// skip, address is already aligned
	strb	w1,[x3],#1	// store byte and increase address
	subs	x2,x2,#1	// loop counter
	bne	2b		// store next byte
	ret			// all done

	// prepare data to store
3:	and	w1,w1,#0xff	// X1 <- mask byte to store
	orr	w1,w1,w1,lsl #8	// X1 <- duplicate to 2 bytes
	orr	w1,w1,w1,lsl #16 // X1 <- duplicate to 4 bytes
	orr	x1,x1,x1,lsl #32 // X1 <- duplicate to 8 bytes

	// store aligned data by 2 double-words
4:	cmp	x2,#16		// check if size is at least 16 bytes
	blt	5f		// insufficient data
	stp	x1,x1,[x3],#16	// store 2 words and increase address
	subs	x2,x2,#16	// loop counter
	bgt	4b		// next data

	// store rest of data by bytes
5:	cbz	x2,9f		// exit if size is 0
6:	strb	w1,[x3],#1	// store byte and increase address
	subs	x2,x2,#1	// loop counter
	bne	6b		// next byte
9:	ret

.ltorg

// ----------------------------------------------------------------------------
//                         Wrap memcpy function
// ----------------------------------------------------------------------------
// The memcpy() library function uses unaligned memory access. This is a problem
// when accessing uncached memory - in which case the library function crashes.
// The following function provides aligned access.
// INPUT:
//  X0 = destination address
//  X1 = source address
//  X2 = length in bytes
// OUTPUT:
//  X0 = original destination address

	.section .text.__wrap_memcpy
	.align	3
.global __wrap_memcpy
__wrap_memcpy:

	// destination address -> X3 (we must return original destination address in X0)
	mov	x3,x0		// X3 <- destination address

	// check small memory block
	cmp	x2,#32		// check if size is at least 32 bytes
	blt	6f		// small block - skip large transfers

	// check identical alignment to 8 bytes
	eor	x4,x3,x1	// X4 <- dst ^ src
	tst	x4,#7		// compare alignment
	bne	6f		// different alignment - whole block must be transfered by bytes

	// copy start of data by bytes
2:	tst	x3,#7		// check if address is aligned to 8 bytes
	beq	4f		// skip, address is already aligned
	ldrb	w4,[x1],#1	// X4 <- load byte and increase address
	subs	x2,x2,#1	// loop counter (delay after ldrb)
	strb	w4,[x3],#1	// store byte and increase address
	bne	2b		// copy next byte
	ret			// all done

	// copy aligned data by 4 double-words
4:	cmp	x2,#32		// check if size is at least 32 bytes
	blt	5f		// insufficient data
	ldp	x4,x5,[x1],#16	// load 2 double-words and increase address
	ldp	x6,x7,[x1],#16	// load 2 double-words and increase address
	subs	x2,x2,#32	// loop counter
	stp	x4,x5,[x3],#16	// store 2 double-words and increase address
	stp	x6,x7,[x3],#16	// store 2 double-words and increase address
	bgt	4b		// next data

	// copy aligned data by words
5:	cmp	x2,#8		// check if size is at least 8 bytes
	blt	6f		// insufficient data
	ldr	x4,[x1],#8	// load word and increase address
	subs	x2,x2,#8	// loop counter (delay after ldr)
	str	x4,[x3],#8	// store word and increase address
	bgt	5b		// next data

	// copy rest of data by bytes
6:	cbz	x2,9f		// exit if size is 0
7:	ldrb	w4,[x1],#1	// X4 <- load byte and increase address
	subs	x2,x2,#1	// loop counter (delay after ldrb)
	strb	w4,[x3],#1	// store byte and increase address
	bne	7b		// next byte

9:	ret

.ltorg

// ----------------------------------------------------------------------------
//                         Wrap memcmp function
// ----------------------------------------------------------------------------
// The memcmp() library function uses unaligned memory access. This is a problem
// when accessing uncached memory - in which case the library function crashes.
// The following function provides aligned access.
// INPUT:
//  X0 = address 1
//  X1 = address 2
//  X2 = length in bytes
// OUTPUT:
//  X0 = difference

	.section .text.__wrap_memcmp
	.align	3
.global __wrap_memcmp
__wrap_memcmp:

	// no data, memory equal
	cbz	x2,6f		// skip if no data

	// compare loop
2:	ldrb	w3,[x0],#1	// X3 <- load byte 1 and increment
	ldrb	w4,[x1],#1	// X4 <- load byte 2 and increment
	cmp	w3,w4		// compare bytes
	bne	8f		// difference
	subs	x2,x2,#1	// byte counter
	bne	2b		// next byte

	// memory equal
6:	mov	w0,#0		// result is 0
	ret

	// memory difference
8:	sub	x0,x3,x4	// X0 <- byte difference
	ret

.ltorg

// ----------------------------------------------------------------------------
//                         Wrap memmove function
// ----------------------------------------------------------------------------
// The memmove() library function uses unaligned memory access. This is a problem
// when accessing uncached memory - in which case the library function crashes.
// The following function provides aligned access.
// INPUT:
//  X0 = destination address
//  X1 = source address
//  X2 = length in bytes
// OUTPUT:
//  X0 = original destination address

	.section .text.__wrap_memmove
	.align	3
.global __wrap_memmove
__wrap_memmove:

	// check if size is 0
	cbz	x2,9f		// exit if size is 0

	// destination address -> R3 (we must return original destination address in R0)
	mov	x3,x0		// R3 <- destination address

	// compare adresses
	cmp	x3,x1		// compare dst ? src
	blo	6f		// if dst < src, copy forward (up)
	beq	9f		// if dst == src, exit

	// copy backward
	add	x1,x1,x2	// X1 <- shift source address to the end
	add	x3,x3,x2	// X3 <- shift destination address to the end
4:	ldrb	w4,[x1,#-1]!	// X4 <- pre-decrement address and get byte
	subs	x2,x2,#1	// loop counter (delay after ldrb)
	strb	w4,[x3,#-1]!	// pre-decrement address and store byte
	bne	4b
	ret

	// copy forward
6:	ldrb	w4,[x1],#1	// X4 <- load byte and increase address
	subs	x2,x2,#1	// loop counter (delay after ldrb)
	strb	w4,[x3],#1	// store byte and increase address
	bne	6b		// next byte
9:	ret

.ltorg
