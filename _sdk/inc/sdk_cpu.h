
// ****************************************************************************
//
//                                 CPU control
//
// ****************************************************************************

#ifndef _SDK_CPU_H
#define _SDK_CPU_H

// ----------------------------------------------------------------------------
//                            CPU control
// ----------------------------------------------------------------------------

// Execution levels
#define TASK_LEVEL	0	// task level - IRQs and FIQs are enabled
#define IRQ_LEVEL	1	// IRQ level - IRQs are disabled, FIQs are enabled
#define FIQ_LEVEL	2	// FIQ level - IRQs and FIQs are disabled

// get current execution level *_LEVEL
INLINE int CurrentExecutionLevel(void)
{
#if AARCH==32
	u32 flags;
	__asm volatile (" mrs %0,cpsr\n" : "=r" (flags));
#else
	u64 flags;
	__asm volatile (" mrs %0,daif\n" : "=r" (flags));
#endif

	if ((flags & B6) != 0) return FIQ_LEVEL;
	if ((flags & B7) != 0) return IRQ_LEVEL;
	return TASK_LEVEL;
}

// get index of current processor core (0..3)
INLINE int CpuID(void)
{
#if AARCH==64
	u64 mpidr;
	__asm volatile (" mrs %0,mpidr_el1\n" : "=r"(mpidr));
	return (u32)(mpidr & (CORES-1));
#elif RASPPI == 1
	return 0;
#else
	u32 mpidr;
	__asm volatile (" mrc p15,0,%0,c0,c0,5\n" : "=r"(mpidr));
	return mpidr & (CORES-1);
#endif
}

// read MIDR register (Zero1, Pi1: 0x410FB767, Pi2v1: 0x410FC075, Zero2, Pi2v2, Pi3: 0x410FD034)
//   bit 0-3: Revision
//   bit 4-15: Part Number
//		0xB76 = BCM2835, ARM1176JZF-S, Armv6 (Pi Zero, Pi 1)
//		0xC07 = BCM2836, Cortex-A7, Armv7 (Pi 2 v1)
//		0xD03 = BCM2837, Cortex-A53, Armv8 (Pi 2 v2, Pi 3, Zero 2)
//		0xD08 = BCM2711, Cortex-A72, Armv8 (Pi 4)
//		0xD0B = BCM2712, Cortex-A76, Armv8 (Pi 5)
//   bit 16-19: Architecture ... not supported on new chips (old chips: 6=Armv6, 7=Armv7, 15=Armv8)
//   bit 20-23: Variant
//   bit 24-31: Implementer (0x41 = ARM)
#if AARCH==32
INLINE u32 ReadMIDR(void)
{
    u32 r;
    __asm volatile(" mrc p15,0,%0,c0,c0,0\n" : "=r"(r));
    return r;
}
#else // AARCH==32
INLINE u64 ReadMIDR(void)
{
    u64 r;
    __asm volatile(" mrs %0,midr_el1\n" : "=r"(r));
    return r;
}
#endif // AARCH==32

// compiler barrier
INLINE void cb(void) { __asm volatile ("" ::: "memory"); }

// no operation (nop instruction)
INLINE void nop(void) { __asm volatile (" nop\n" ::: "memory"); }

#if AARCH==32

// enable fast interrupts FIQ
INLINE void ef(void) { __asm volatile (" cpsie f\n" : : : "memory"); }

// disable fast interrupts FIQ
INLINE void df(void) { __asm volatile (" cpsid f\n" : : : "memory"); }

// enable global interrupts IRQ (not FIQ)
INLINE void ei(void) { __asm volatile (" cpsie i\n" : : : "memory"); }

// disable global interrupts IRQ (not FIQ)
INLINE void di(void) { __asm volatile (" cpsid i\n" : : : "memory"); }

// enable global interrupts IRQ and fast interrupts FIQ
INLINE void eif(void) { __asm volatile (" cpsie if\n" : : : "memory"); }

// disable global interrupts IRQ and fast interrupts FIQ
INLINE void dif(void) { __asm volatile (" cpsid if\n" : : : "memory"); }

// get global interrupts IRQ (returns True if interrupts are enabled)
INLINE Bool geti(void)
{
	u32 flags;
	__asm volatile (" mrs %0,cpsr\n" : "=r" (flags) :: "memory");
	return (Bool)((flags & B7) == 0);
}

// get fast interrupts FIQ (returns True if interrupts are enabled)
INLINE Bool getf(void)
{
	u32 flags;
	__asm volatile (" mrs %0,cpsr\n" : "=r" (flags) :: "memory");
	return (Bool)((flags & B6) == 0);
}

// save IRQ and FIQ state (after it, call di/df/dif function)
INLINE u32 irqfiq_save(void)
{
	u32 flags;
	__asm volatile (" mrs %0,cpsr\n" : "=r" (flags) :: "memory");
	return flags;
}

// restore IRQ and FIQ state
// The following bits will also be restored:
//  bit 0-4 M ... CPU mode (User, FIQ, IRQ, SVC, ABT, UND, SYS)
//  bit 5 T ... thumb bit
//  bit 6 F ... FIQ mask
//  bit 7 I ... IRQ mask
//  bit 8 A ... assync aborts
//  bit 9 E ... endianness
INLINE void irqfiq_restore(u32 flags)
{
    __asm volatile(" msr cpsr_c,%0\n" :: "r"(flags) : "memory");
}

// invalidate instruction cache
INLINE void InvalidateInstructionCache(void)
{
	__asm volatile (" mcr p15,0,%0,c7,c5,0\n" : : "r" (0) : "memory");
}

// flush branch target cache
INLINE void FlushBranchTargetCache(void)
{
	__asm volatile (" mcr p15,0,%0,c7,c5,6\n" : : "r" (0) : "memory");
}

// clean and invalidate data cache in range
extern "C" void CleanAndInvalidateDataCacheRange (u32 addr, u32 len);

#if RASPPI == 1

// instruction synchronization barrier
INLINE void isb(void) { __asm volatile (" mcr p15,0,%0,c7,c5,4\n" :: "r" (0) : "memory"); }

// data memory barrier
INLINE void dmb(void) { __asm volatile (" mcr p15,0,%0,c7,c10,5\n" :: "r" (0) : "memory"); }

// data memory barrier inner-shareable
INLINE void dmb_ish(void) { dmb(); }

// data synchronization barrier
INLINE void dsb(void) { __asm volatile (" mcr p15,0,%0,c7,c10,4\n" :: "r" (0) : "memory"); }

// wait for interrupt
INLINE void wfi(void) {}

// wait for event
INLINE void wfe(void) {}

// send event
INLINE void sev(void) {}

// flush prefetch buffer
INLINE void FlushPrefetchBuffer(void)
{
	__asm volatile (" mcr p15,0,%0,c7,c5,4\n" : : "r" (0) : "memory");
}

// invalidate data cache (with data sync barrier)
INLINE void InvalidateDataCache(void)
{
	__asm volatile (" mcr p15,0,%0,c7,c6,0\n"
		      " mcr p15,0,%0,c7,c10,4\n" : : "r" (0) : "memory");
}

// clean data cache (with data sync barrier)
INLINE void CleanDataCache(void)
{
	__asm volatile (" mcr p15,0,%0,c7,c10,0\n"
		      " mcr p15,0,%0,c7,c10,4\n" : : "r" (0) : "memory");
}

#else // RASPPI == 1

// instruction synchronization barrier
INLINE void isb(void) { __asm volatile (" isb\n" ::: "memory"); }

// data memory barrier
INLINE void dmb(void) { __asm volatile (" dmb sy\n" ::: "memory"); }

// data memory barrier inner-shareable
INLINE void dmb_ish(void) { __asm volatile (" dmb ish\n" ::: "memory"); }

// data synchronization barrier
INLINE void dsb(void) { __asm volatile (" dsb\n" ::: "memory"); }

// wait for interrupt
INLINE void wfi(void) { __asm volatile (" wfi\n" ::: "memory"); }

// wait for event
INLINE void wfe(void) { __asm volatile (" wfe\n" ::: "memory"); }

// send event
INLINE void sev(void) { __asm volatile (" sev\n" ::: "memory"); }

// Invalidate data cache L1
extern "C" void InvalidateDataCacheL1Only(void);

// invalidate data cache (with data sync barrier)
extern "C" void InvalidateDataCache(void);

// clean data cache (with data sync barrier)
extern "C" void CleanDataCache(void);

#endif // RASPPI == 1

#else // AARCH==32

// enable fast interrupts FIQ
INLINE void ef(void) { __asm volatile (" msr DAIFClr,#1\n" : : : "memory"); }

// disable fast interrupts FIQ
INLINE void df(void) { __asm volatile (" msr DAIFSet,#1\n" : : : "memory"); }

// enable global interrupts IRQ (not FIQ)
INLINE void ei(void) { __asm volatile (" msr DAIFClr,#2\n" : : : "memory"); }

// disable global interrupts IRQ (not FIQ)
INLINE void di(void) { __asm volatile (" msr DAIFSet,#2\n" : : : "memory"); }

// enable global interrupts IRQ and fast interrupts FIQ
INLINE void eif(void) { __asm volatile (" msr DAIFClr,#3\n" : : : "memory"); }

// disable global interrupts IRQ and fast interrupts FIQ
INLINE void dif(void) { __asm volatile (" msr DAIFSet,#3\n" : : : "memory"); }

// get global interrupts IRQ (returns True if interrupts are enabled)
INLINE Bool geti(void)
{
	u64 flags;
	__asm volatile (" mrs %0,DAIF\n" : "=r" (flags) :: "memory");
	return (Bool)((flags & B1) == 0);
}

// get fast interrupts FIQ (returns True if interrupts are enabled)
INLINE Bool getf(void)
{
	u64 flags;
	__asm volatile (" mrs %0,DAIF\n" : "=r" (flags) :: "memory");
	return (Bool)((flags & B0) == 0);
}

// save IRQ and FIQ state (after it, call di/df/dif function)
INLINE u32 irqfiq_save(void)
{
	u32 flags;
	__asm volatile (" mrs %0,DAIF\n" : "=r" (flags) :: "memory");
	return flags;
}

// restore IRQ and FIQ state
// The following bits will also be restored: D (Debug mask), A (SError mask), I (IRQ mask), F (FIQ mask)
INLINE void irqfiq_restore(u32 flags)
{
    __asm volatile(" msr DAIF,%0\n" :: "r"(flags) : "memory");
}

// instruction synchronization barrier
INLINE void isb(void) { __asm volatile (" isb\n" ::: "memory"); }

// data memory barrier
INLINE void dmb(void) { __asm volatile (" dmb sy\n" ::: "memory"); }

// data memory barrier inner-shareable
INLINE void dmb_ish(void) { __asm volatile (" dmb ish\n" ::: "memory"); }

// data synchronization barrier
INLINE void dsb(void) { __asm volatile (" dsb sy\n" ::: "memory"); }

// wait for interrupt
INLINE void wfi(void) { __asm volatile (" wfi\n" ::: "memory"); }

// wait for event
INLINE void wfe(void) { __asm volatile (" wfe\n" ::: "memory"); }

// send event
INLINE void sev(void) { __asm volatile (" sev\n" ::: "memory"); }

// invalidate instruction cache
INLINE void InvalidateInstructionCache(void) { __asm volatile (" ic iallu\n" ::: "memory"); }

// Invalidate data cache L1
extern "C" void InvalidateDataCacheL1Only(void);

// invalidate data cache (with data sync barrier)
extern "C" void InvalidateDataCache(void);

// clean data cache (with data sync barrier)
extern "C" void CleanDataCache(void);

// invalidate data cache in range
void InvalidateDataCacheRange(u64 addr, u64 len);

// clean data cache in range
void CleanDataCacheRange(u64 addr, u64 len);

// clean and invalidate data cache in range
extern "C" void CleanAndInvalidateDataCacheRange(u64 addr, u64 len);

#endif // AARCH==32

// clean caches
void SyncDataAndInstructionCache(void);

// set global interrupts IRQ (True to enable interrupts) ... not FIQ
INLINE void seti(Bool enable)
{
	if (enable) ei(); else di();	
}

// set fast interrupts FIQ (True to enable interrupts)
INLINE void setf(Bool enable)
{
	if (enable) ef(); else df();	
}

// disable global interrupts IRQ (not FIQ), returns previous state
INLINE u32 LockIRQ(void)
{
	u32 flags = irqfiq_save();
	di();
	return flags;
}

// disable global interrupts IRQ and fast interrupts FIQ, returns previous state
INLINE u32 LockIRQFIQ(void)
{
	u32 flags = irqfiq_save();
	dif();
	return flags;
}

// restore interrupts
INLINE void UnlockIRQ(u32 flags)
{
	irqfiq_restore(flags);
}

// restore interrupts
INLINE void UnlockIRQFIQ(u32 flags)
{
	irqfiq_restore(flags);
}

// IRQ lock (temporary disables global interrupts IRQ, not FIQ)
#define IRQ_LOCK u32 irq_state = LockIRQ()	// lock on begin of critical section
#define IRQ_RELOCK irq_state = LockIRQ()	// lock again on begin of critical section
#define IRQ_UNLOCK UnlockIRQ(irq_state)		// unlock on end of critical section

// IRQ and FIQ lock (temporary disables global interrupts IRQ and fast interrupts FIQ)
#define IRQFIQ_LOCK u32 irqfiq_state = LockIRQFIQ()	// lock on begin of critical section
#define IRQFIQ_RELOCK irqfiq_state = LockIRQFIQ()	// lock again on begin of critical section
#define IRQFIQ_UNLOCK UnlockIRQFIQ(irqfiq_state)	// unlock on end of critical section

// ----------------------------------------------------------------------------
//                            Bit manipulations
// ----------------------------------------------------------------------------

// get number of leading zeros of u32 number
INLINE u32 clz(u32 val)
{
//	return __builtin_clz(val);
#if RASPPI == 1
	if (val == 0) return 32;
	u32 n = 0;
	if ((val & 0xFFFF0000u) == 0) { n += 16; val <<= 16; }
	if ((val & 0xFF000000u) == 0) { n += 8; val <<= 8; }
	if ((val & 0xF0000000u) == 0) { n += 4; val <<= 4; } 
	if ((val & 0xC0000000u) == 0) { n += 2; val <<= 2; }
	if ((val & 0x80000000u) == 0) { n += 1; }
	return n;
#elif AARCH == 64
	u32 res;
	__asm volatile (" clz %w0,%w1\n" : "=r" (res) : "r" (val));
	return res;
#else
	u32 res;
	__asm volatile (" clz %0,%1\n" : "=r" (res) : "r" (val));
	return res;
#endif
}

// get number of leading zeros of u64 number
INLINE u32 clz64(u64 val)
{
#if AARCH == 64
	u64 res;
	__asm volatile (" clz %0,%1\n" : "=r" (res) : "r" (val));
	return (u32)res;
#else
	u32 hi = (u32)(val >> 32);
	if (hi != 0)
		return clz(hi);
	else
		return clz((u32)val) + 32;
#endif
}

// get number of trailing zeros of u32 number
INLINE u32 ctz(u32 val)
{
	return __builtin_ctz(val);
}

// get number of trailing zeros of u64 number
INLINE u32 ctz64(u64 val)
{
	return __builtin_ctzll(val);
}

// get bit order of 8-bit value (logarithm, returns position of highest bit + 1: 1..8, 0=no bit)
INLINE u32 Order8(u8 val) { return 32 - clz((u32)(u8)val); }

// get bit order of value (logarithm, returns position of highest bit + 1: 1..32, 0=no bit)
INLINE u32 Order(u32 val) { return 32 - clz(val); }

// get bit order of 64-bit value (logarithm, returns position of highest bit + 1: 1..64, 0=no bit)
INLINE u32 Order64(u64 val)
{
	u32 hi = (u32)(val >> 32);
	if (hi != 0)
		return Order(hi) + 32;
	else
		return Order((u32)val);
}

// get mask of value (0x123 returns 0x1FF)
INLINE u32 Mask(u32 val) { return ((u32)-1) >> clz(val); }

// range mask - returns bits set to '1' on range 'first' to 'last' (RangeMask(7,14) returns 0x7F80)
INLINE u32 RangeMask(int first, int last) { return (~(((u32)-1) << (last+1))) & (((u32)-1) << first); }

// range mask - returns bits set to '1' on range 'first' to 'last' (RangeMask(7,14) returns 0x7F80)
INLINE u64 RangeMask64(int first, int last) { return (~(((u64)-1) << (last+1))) & (((u64)-1) << first); }

// check if integer number is power of 2
INLINE Bool IsPow2(u32 a) { return ((a & (a-1)) == 0); }

// reverse byte order of DWORD (exchange Big-endian and Little-endian byte order)
// (little endian LSB MSB = Intel, big endian MSB LSB = Motorola)
INLINE u32 Endian(u32 val)
{
//	return __builtin_bswap32(val);
	u32 res;
#if AARCH == 64
	__asm volatile (" rev %w0,%w1\n" : "=r" (res) : "r" (val));
#else
	__asm volatile (" rev %0,%1\n" : "=r" (res) : "r" (val));
#endif
	return res;
}

INLINE u64 Endian64(u64 val)
{
	u64 res;
#if AARCH == 64
	__asm volatile (" rev %0,%1\n" : "=r" (res) : "r" (val));
#else
	u32 hi = (u32)(val >> 32);
	u32 lo = (u32)val;
	res = ((u64)Endian(lo) << 32) | (u64)Endian(hi);
#endif
	return res;
}

// swap bytes of WORD (exchange Big-endian and Little-endian byte order)
// (little endian LSB MSB = Intel, big endian MSB LSB = Motorola)
INLINE u16 Swap(u16 val)
{
//	return __builtin_bswap16(val);
	u32 tmp;
#if AARCH == 64
	__asm volatile (" rev16 %w0,%w1\n" : "=r" (tmp) : "r" ((u32)val));
#else
	__asm volatile (" rev16 %0,%1\n" : "=r" (tmp) : "r" ((u32)val));
#endif
	return (u16)tmp;
}

// swap bytes in two WORDs (exchange Big-endian and Little-endian byte order)
// (little endian LSB MSB = Intel, big endian MSB LSB = Motorola)
INLINE u32 Swap2(u32 val)
{
//	return __builtin_bswap16((u16)val) | ((u32)__builtin_bswap16((u16)(val >> 16)) << 16);
	u32 res;
#if AARCH == 64
	__asm volatile (" rev16 %w0,%w1\n" : "=r" (res) : "r" (val));
#else
	__asm volatile (" rev16 %0,%1\n" : "=r" (res) : "r" (val));
#endif
	return res;
}

// rotate bits in DWORD right by 'num' bits (carry lower bits to higher bits)
INLINE u32 Ror(u32 val, u8 num)
{
	num &= 0x1f;
	u32 res;
#if AARCH == 64
	__asm volatile (" ror %w0,%w1,%w2\n" : "=r" (res) : "r" (val), "r" (num));
#else
	__asm volatile (" ror %0,%1,%2\n" : "=r" (res) : "r" (val), "r" (num));
#endif
	return res;
}

// rotate bits in DWORD left by 'num' bits (carry higher bits to lower bits)
INLINE u32 Rol(u32 val, u8 num)
{
	num &= 0x1f;
	if (num == 0) return val;
	u32 res;
#if AARCH == 64
	__asm volatile (" ror %w0,%w1,%w2\n" : "=r" (res) : "r" (val), "r" (32 - num));
#else
	__asm volatile (" ror %0,%1,%2\n" : "=r" (res) : "r" (val), "r" (32 - num));
#endif
	return res;
}

// rotate bits in u64 right by 'num' bits (carry lower bits to higher bits)
INLINE u64 Ror64(u64 val, u8 num)
{
	num &= 0x3f;
	u64 res;
#if AARCH == 64
	__asm volatile (" ror %0,%1,%2\n" : "=r" (res) : "r" (val), "r" (num));
#else
	return (val >> num) | (val << (64 - num));
#endif
	return res;
}

// rotate bits in u64 left by 'num' bits (carry higher bits to lower bits)
INLINE u64 Rol64(u64 val, u8 num)
{
	num &= 0x3f;
	if (num == 0) return val;
	u64 res;
#if AARCH == 64
	__asm volatile (" ror %0,%1,%2\n" : "=r" (res) : "r" (val), "r" (64 - num));
#else
	return (val << num) | (val >> (64 - num));
#endif
	return res;
}

// reverse 32 bits
INLINE u32 Reverse32(u32 val)
{
	u32 res;
#if AARCH == 64
	__asm volatile (" rbit %w0,%w1\n" : "=r" (res) : "r" (val));
#elif RASPPI == 1
	val = ((val & 0x55555555u) << 1) | ((val >> 1) & 0x55555555u);
	val = ((val & 0x33333333u) << 2) | ((val >> 2) & 0x33333333u);
	val = ((val & 0x0F0F0F0Fu) << 4) | ((val >> 4) & 0x0F0F0F0Fu);
	val = ((val & 0x00FF00FFu) << 8) | ((val >> 8) & 0x00FF00FFu);
	val = (val >> 16) | (val << 16);
	return val;
#else
	__asm volatile (" rbit %0,%1\n" : "=r" (res) : "r" (val));
#endif
	return res;
}

// reverse 16 bits
INLINE u16 Reverse16(u16 val) { return (u16)(Reverse32((u32)val) >> 16); }

// reverse 8 bits
INLINE u8 Reverse8(u8 val) { return (u8)(Reverse32((u32)val) >> 24); }

// reverse 64 bits
INLINE u64 Reverse64(u64 val)
{
	u64 res;
#if AARCH == 64
	__asm volatile (" rbit %0,%1\n" : "=r" (res) : "r" (val));
	return res;
#else
	return ((u64)Reverse32((u32)val) << 32) | (u64)Reverse32((u32)(val >> 32));
#endif
}

INLINE u32 reverse(u32 val) { return Reverse32(val); }

INLINE u32 __rev(u32 val) { return Reverse32(val); }

INLINE u64 __revll(u64 val) { return Reverse64(val); }

// counts '1' bits (population count)
INLINE u32 Popcount(u32 val) { return __builtin_popcount(val); }
INLINE u32 popcount(u32 val) { return Popcount(val); }
INLINE u32 Popcount64(u64 val)
{
#if AARCH == 64
	return __builtin_popcountll(val);
#else
	return Popcount((u32)val) + Popcount((u32)(val >> 32));
#endif
}

// ----------------------------------------------------------------------------
//                             Atomic operations
// ----------------------------------------------------------------------------

#if MMU_CACHE_MODE == 2		// MMU cache mode: 0=all cached (default), 1=video not cached, 2=no cache

// Atomic operations are not supported on non-cached memory - use only simple simulation
INLINE u8 GetLock8(volatile u8* addr) { return *addr; }
INLINE u16 GetLock16(volatile u16* addr) { return *addr; }
INLINE u32 GetLock32(volatile u32* addr) { return *addr; }
INLINE u64 GetLock64(volatile u64* addr) { return *addr; }
INLINE Bool SetLock8(volatile u8* addr, u8 val) { *addr = val; return False; }
INLINE Bool SetLock16(volatile u16* addr, u16 val) { *addr = val; return False; }
INLINE Bool SetLock32(volatile u32* addr, u32 val) { *addr = val; return False; }
INLINE Bool SetLock64(volatile u64* addr, u64 val) { *addr = val; return False; }
INLINE void ClrLock() {}

#else // MMU_CACHE_MODE

#if AARCH==32

// load byte exclusive (must precede before save byte exclusive; volatile variable should be used)
INLINE u8 GetLock8(volatile u8* addr)
{
	u8 res;
	__asm volatile (" ldrexb %0,[%1]\n" : "=r" (res) : "r" (addr));
	return res;
}

// load word exclusive (must precede before save word exclusive; volatile variable should be used)
INLINE u16 GetLock16(volatile u16* addr)
{
	u16 res;
	__asm volatile (" ldrexh %0,[%1]\n" : "=r" (res) : "r" (addr));
	return res;
}

// load dword exclusive (must precede before save dword exclusive; volatile variable should be used)
INLINE u32 GetLock32(volatile u32* addr)
{
	u32 res;
	__asm volatile (" ldrex %0,[%1]\n" : "=r" (res) : "r" (addr));
	return res;
}

// load qword exclusive (must precede before save dword exclusive; volatile variable should be used)
// ... not supported on 32-bit mode (only simulation)
INLINE u64 GetLock64(volatile u64* addr) { return *addr; }

// save byte exclusive (returns True if failed, must follow after load byte exclusive; volatile variable should be used)
INLINE Bool SetLock8(volatile u8* addr, u8 val)
{
	u32 res;
	__asm volatile (" strexb %0,%2,[%1]\n" : "=&r" (res) : "r" (addr), "r" (val) : "memory");
	return (Bool)res;
}

// save word exclusive (returns True if failed, must follow after load word exclusive; volatile variable should be used)
INLINE Bool SetLock16(volatile u16* addr, u16 val)
{
	u32 res;
	__asm volatile (" strexh %0,%2,[%1]\n" : "=&r" (res) : "r" (addr), "r" (val) : "memory");
	return (Bool)res;
}

// save dword exclusive (returns True if failed, must follow after load word exclusive; volatile variable should be used)
INLINE Bool SetLock32(volatile u32* addr, u32 val)
{
	u32 res;
	__asm volatile (" strex %0,%2,[%1]\n" : "=&r" (res) : "r" (addr), "r" (val) : "memory");
	return (Bool)res;
}

// save qword exclusive (returns True if failed, must follow after load word exclusive; volatile variable should be used)
// ... not supported on 32-bit mode (only simulation)
INLINE Bool SetLock64(volatile u64* addr, u64 val) { *addr = val; return False; }

#else // AARCH==32

// load byte exclusive (must precede before save byte exclusive; volatile variable should be used)
INLINE u8 GetLock8(volatile u8* addr)
{
	u8 res;
	__asm volatile (" ldxrb %w0,[%1]\n" : "=r" (res) : "r" (addr));
	return res;
}

// load word exclusive (must precede before save word exclusive; volatile variable should be used)
INLINE u16 GetLock16(volatile u16* addr)
{
	u16 res;
	__asm volatile (" ldxrh %w0,[%1]\n" : "=r" (res) : "r" (addr));
	return res;
}

// load dword exclusive (must precede before save dword exclusive; volatile variable should be used)
INLINE u32 GetLock32(volatile u32* addr)
{
	u32 res;
	__asm volatile (" ldxr %w0,[%1]\n" : "=r" (res) : "r" (addr));
	return res;
}

// load qword exclusive (must precede before save dword exclusive; volatile variable should be used)
INLINE u64 GetLock64(volatile u64* addr)
{
	u64 res;
	__asm volatile (" ldxr %0,[%1]\n" : "=r" (res) : "r" (addr));
	return res;
}

// save byte exclusive (returns True if failed, must follow after load byte exclusive; volatile variable should be used)
INLINE Bool SetLock8(volatile u8* addr, u8 val)
{
	u32 res;
	__asm volatile (" stxrb %w0,%w2,[%1]\n" : "=&r" (res) : "r" (addr), "r" (val) : "memory");
	return (Bool)res;
}

// save word exclusive (returns True if failed, must follow after load word exclusive; volatile variable should be used)
INLINE Bool SetLock16(volatile u16* addr, u16 val)
{
	u32 res;
	__asm volatile (" stxrh %w0,%w2,[%1]\n" : "=&r" (res) : "r" (addr), "r" (val) : "memory");
	return (Bool)res;
}

// save dword exclusive (returns True if failed, must follow after load word exclusive; volatile variable should be used)
INLINE Bool SetLock32(volatile u32* addr, u32 val)
{
	u32 res;
	__asm volatile (" stxr %w0,%w2,[%1]\n" : "=&r" (res) : "r" (addr), "r" (val) : "memory");
	return (Bool)res;
}

// save qword exclusive (returns True if failed, must follow after load word exclusive; volatile variable should be used)
INLINE Bool SetLock64(volatile u64* addr, u64 val)
{
	u32 res;
	__asm volatile (" stxr %w0,%2,[%1]\n" : "=&r" (res) : "r" (addr), "r" (val) : "memory");
	return (Bool)res;
}

#endif // AARCH==32

// cancel exclusive lock (folowing save exclusive will fail)
INLINE void ClrLock()
{
	__asm volatile (" clrex\n" ::: "memory");
}

#endif // MMU_CACHE_MODE

// exchange byte exclusive (returns old value)
INLINE u8 ExcLock8(volatile u8* addr, u8 val)
{
	u8 oldval;
	do {
		oldval = GetLock8(addr);
	} while (SetLock8(addr, val));
	return oldval;
}

// exchange word exclusive (returns old value)
INLINE u16 ExcLock16(volatile u16* addr, u16 val)
{
	u16 oldval;
	do {
		oldval = GetLock16(addr);
	} while (SetLock16(addr, val));
	return oldval;
}

// exchange dword exclusive (returns old value)
INLINE u32 ExcLock32(volatile u32* addr, u32 val)
{
	u32 oldval;
	do {
		oldval = GetLock32(addr);
	} while (SetLock32(addr, val));
	return oldval;
}

// exchange qword exclusive (returns old value)
// ... not supported on 32-bit mode (only simulation)
INLINE u64 ExcLock64(volatile u64* addr, u64 val)
{
	u64 oldval;
	do {
		oldval = GetLock64(addr);
	} while (SetLock64(addr, val));
	return oldval;
}

// increment byte with exclusive lock (returns resulting value)
INLINE u8 IncLock8(volatile u8* addr)
{
	u8 val;
	do {
		val = GetLock8(addr) + 1;
	} while (SetLock8(addr, val));
	return val;
}

// increment word with exclusive lock (returns resulting value)
INLINE u16 IncLock16(volatile u16* addr)
{
	u16 val;
	do {
		val = GetLock16(addr) + 1;
	} while (SetLock16(addr, val));
	return val;
}

// increment dword with exclusive lock (returns resulting value)
INLINE u32 IncLock32(volatile u32* addr)
{
	u32 val;
	do {
		val = GetLock32(addr) + 1;
	} while (SetLock32(addr, val));
	return val;
}

// increment qword with exclusive lock (returns resulting value)
// ... not supported on 32-bit mode (only simulation)
INLINE u64 IncLock64(volatile u64* addr)
{
	u64 val;
	do {
		val = GetLock64(addr) + 1;
	} while (SetLock64(addr, val));
	return val;
}

// decrement byte with exclusive lock (returns resulting value)
inline u8 DecLock8(volatile u8* addr)
{
	u8 val;
	do {
		val = GetLock8(addr) - 1;
	} while (SetLock8(addr, val));
	return val;
}

// decrement word with exclusive lock (returns resulting value)
inline u16 DecLock16(volatile u16* addr)
{
	u16 val;
	do {
		val = GetLock16(addr) - 1;
	} while (SetLock16(addr, val));
	return val;
}

// decrement dword with exclusive lock (returns resulting value)
inline u32 DecLock32(volatile u32* addr)
{
	u32 val;
	do {
		val = GetLock32(addr) - 1;
	} while (SetLock32(addr, val));
	return val;
}

// decrement qword with exclusive lock (returns resulting value)
// ... not supported on 32-bit mode (only simulation)
inline u64 DecLock64(volatile u64* addr)
{
	u64 val;
	do {
		val = GetLock64(addr) - 1;
	} while (SetLock64(addr, val));
	return val;
}

// ----------------------------------------------------------------------------
//                           Long Jump
// ----------------------------------------------------------------------------

// jmp_buf buffer
#if AARCH==32

// Callee-saved CPU registers of 32-bit:
//  R4-R9, R10=SL, R11=FP, R13=SP, R14=LR ... 10 registers u32
//  D8-D15 ... 8 registers double
// ... total 26 values u32 or 13 values u64
#define LONGJUMP_LEN	13
typedef u64 jmp_buf[LONGJUMP_LEN];

#else // AARCH==32

// Callee-saved CPU registers of 64-bit:
//  X19-X30, SP ... 13 registers u64 + 1 empty slot
//  D8-D15 ... 8 registers double
// ... total 22 values u64
#define LONGJUMP_LEN	22
typedef u64 jmp_buf[LONGJUMP_LEN];

#endif // AARCH==32

// Set long jump
//  env ... jmp_buf object
// Returns: 0=first calling of setjmp(), <>0=return from longjmp() function
extern "C" int setjmp(jmp_buf env);

// Execute long jump
//  env ... jmp_buf object
//  val ... return value, should not be 0 (returns 1 if val=0)
extern "C" void longjmp(jmp_buf env, int val);

#endif // _SDK_CPU_H
