
// ****************************************************************************
//
//                                 CPU control
//
// ****************************************************************************
// Some parts of the code were taken from the Circle library:
//
// synchronize.cpp
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2014-2023  R. Stange <rsta2@o2online.de>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "../../includes.h"	// includes

// clean caches
void SyncDataAndInstructionCache(void)
{
	CleanDataCache();
	InvalidateInstructionCache();
#if AARCH==32
	FlushBranchTargetCache();
#endif
	dsb();
	isb();
}

#if RASPPI == 1

// clean and invalidate data cache in range
void CleanAndInvalidateDataCacheRange(u32 addr, u32 len)
{
	// data cache line length = 32
	u32 start = addr & ~31;		// round address down to start line
	len += addr - start;		// length of range, round up
	while (True)
	{ 
		// clean cache of this line
		__asm volatile (" mcr p15,0,%0,c7,c14,1\n" : : "r" (addr) : "memory");

		// end
		if (len <= 32) break;

		// shift to next line
		addr += 32;
		len -= 32;
	}
	dsb();
}

#endif // RASPPI == 1

#if AARCH==64

#define DATA_CACHE_LINE_LENGTH		64

#if RASPPI == 3		// Pi 3, Zero 2

#define SETWAY_LEVEL_SHIFT		1
#define L1_DATA_CACHE_SETS		128
#define L1_DATA_CACHE_WAYS		4
#define L1_SETWAY_WAY_SHIFT		30	// 32-Log2(L1_DATA_CACHE_WAYS)
#define L1_DATA_CACHE_LINE_LENGTH	64
#define L1_SETWAY_SET_SHIFT		6	// Log2(L1_DATA_CACHE_LINE_LENGTH)
#define L2_CACHE_SETS			512
#define L2_CACHE_WAYS			16
#define L2_SETWAY_WAY_SHIFT		28	// 32-Log2(L2_CACHE_WAYS)
#define L2_CACHE_LINE_LENGTH		64
#define L2_SETWAY_SET_SHIFT		6	// Log2(L2_CACHE_LINE_LENGTH)

#elif RASPPI == 4	// Pi 4

#define SETWAY_LEVEL_SHIFT		1
#define L1_DATA_CACHE_SETS		256
#define L1_DATA_CACHE_WAYS		2
#define L1_SETWAY_WAY_SHIFT		31	// 32-Log2(L1_DATA_CACHE_WAYS)
#define L1_DATA_CACHE_LINE_LENGTH	64
#define L1_SETWAY_SET_SHIFT		6	// Log2(L1_DATA_CACHE_LINE_LENGTH)
#define L2_CACHE_SETS			1024
#define L2_CACHE_WAYS			16
#define L2_SETWAY_WAY_SHIFT		28	// 32-Log2(L2_CACHE_WAYS)
#define L2_CACHE_LINE_LENGTH		64
#define L2_SETWAY_SET_SHIFT		6	// Log2(L2_CACHE_LINE_LENGTH)

#else			// Pi 5

#define SETWAY_LEVEL_SHIFT		1

#define L1_DATA_CACHE_SETS		256
#define L1_DATA_CACHE_WAYS		4
#define L1_SETWAY_WAY_SHIFT		30	// 32-Log2(L1_DATA_CACHE_WAYS)
#define L1_DATA_CACHE_LINE_LENGTH	64
#define L1_SETWAY_SET_SHIFT		6	// Log2(L1_DATA_CACHE_LINE_LENGTH)
#define L2_CACHE_SETS			1024
#define L2_CACHE_WAYS			8
#define L2_SETWAY_WAY_SHIFT		29	// 32-Log2(L2_CACHE_WAYS)
#define L2_CACHE_LINE_LENGTH		64
#define L2_SETWAY_SET_SHIFT		6	// Log2(L2_CACHE_LINE_LENGTH)
#define L3_CACHE_SETS			2048
#define L3_CACHE_WAYS			16
#define L3_SETWAY_WAY_SHIFT		28	// 32-Log2(L2_CACHE_WAYS)
#define L3_CACHE_LINE_LENGTH		64
#define L3_SETWAY_SET_SHIFT		6	// Log2(L2_CACHE_LINE_LENGTH)

#endif

// invalidate L1 data cache (with data sync barrier)
void InvalidateDataCache(void)
{
	int set, way;
	u64 level;

	// invalidate L1 data cache
	for (set = 0; set < L1_DATA_CACHE_SETS; set++)
	{
		for (way = 0; way < L1_DATA_CACHE_WAYS; way++)
		{
			level = way << L1_SETWAY_WAY_SHIFT
				| set << L1_SETWAY_SET_SHIFT
				| 0 << SETWAY_LEVEL_SHIFT;

			__asm volatile ("dc isw,%0\n" : : "r" (level) : "memory");
		}
	}

	// invalidate L2 unified cache
	for (set = 0; set < L2_CACHE_SETS; set++)
	{
		for (way = 0; way < L2_CACHE_WAYS; way++)
		{
			level = way << L2_SETWAY_WAY_SHIFT
				| set << L2_SETWAY_SET_SHIFT
				| 1 << SETWAY_LEVEL_SHIFT;

			__asm volatile ("dc isw,%0\n" : : "r" (level) : "memory");
		}
	}

#ifdef L3_CACHE_SETS
	// invalidate L3 unified cache
	for (set = 0; set < L3_CACHE_SETS; set++)
	{
		for (way = 0; way < L3_CACHE_WAYS; way++)
		{
			level = way << L3_SETWAY_WAY_SHIFT
				| set << L3_SETWAY_SET_SHIFT
				| 1 << SETWAY_LEVEL_SHIFT;

			__asm volatile (" dc isw,%0\n" : : "r" (level) : "memory");
		}
	}
#endif

	dsb();
}

// Invalidate data cache L1
void InvalidateDataCacheL1Only(void)
{
	int set, way;
	u64 level;

	// invalidate L1 data cache
	for (set = 0; set < L1_DATA_CACHE_SETS; set++)
	{
		for (way = 0; way < L1_DATA_CACHE_WAYS; way++)
		{
			level = way << L1_SETWAY_WAY_SHIFT
				| set << L1_SETWAY_SET_SHIFT
				| 0 << SETWAY_LEVEL_SHIFT;

			__asm volatile (" dc isw,%0\n" : : "r" (level) : "memory");
		}
	}

	dsb();
}

// clean data cache (with data sync barrier)
void CleanDataCache(void)
{
	int set, way;
	u64 level;

	// clean L1 data cache
	for (set = 0; set < L1_DATA_CACHE_SETS; set++)
	{
		for (way = 0; way < L1_DATA_CACHE_WAYS; way++)
		{
			level = way << L1_SETWAY_WAY_SHIFT
				| set << L1_SETWAY_SET_SHIFT
				| 0 << SETWAY_LEVEL_SHIFT;

			__asm volatile (" dc csw,%0\n" : : "r" (level) : "memory");
		}
	}

	// clean L2 unified cache
	for (set = 0; set < L2_CACHE_SETS; set++)
	{
		for (way = 0; way < L2_CACHE_WAYS; way++)
		{
			level = way << L2_SETWAY_WAY_SHIFT
				| set << L2_SETWAY_SET_SHIFT
				| 1 << SETWAY_LEVEL_SHIFT;

			__asm volatile (" dc csw,%0\n" : : "r" (level) : "memory");
		}
	}

#ifdef L3_CACHE_SETS
	// clean L3 unified cache
	for (set = 0; set < L3_CACHE_SETS; set++)
	{
		for (way = 0; way < L3_CACHE_WAYS; way++)
		{
			level = way << L3_SETWAY_WAY_SHIFT
				| set << L3_SETWAY_SET_SHIFT
				| 1 << SETWAY_LEVEL_SHIFT;

			__asm volatile (" dc csw,%0\n" : : "r" (level) : "memory");
		}
	}
#endif

	dsb();
}

// invalidate data cache in range
void InvalidateDataCacheRange(u64 addr, u64 len)
{
	u64 line = addr & (~(DATA_CACHE_LINE_LENGTH - 1));
	len += addr - line;
	addr = line;
	while (True)
	{
		__asm volatile (" dc ivac,%0\n" : : "r" (addr) : "memory");

		if (len <= DATA_CACHE_LINE_LENGTH) break;

		addr += DATA_CACHE_LINE_LENGTH;
		len  -= DATA_CACHE_LINE_LENGTH;
	}

	dsb();
}

// clean data cache in range
void CleanDataCacheRange(u64 addr, u64 len)
{
	u64 line = addr & (~(DATA_CACHE_LINE_LENGTH - 1));
	len += addr - line;
	addr = line;
	while (True)
	{
		__asm volatile (" dc cvac,%0\n" : : "r" (addr) : "memory");

		if (len <= DATA_CACHE_LINE_LENGTH) break;

		addr += DATA_CACHE_LINE_LENGTH;
		len  -= DATA_CACHE_LINE_LENGTH;
	}

	dsb();
}

// clean and invalidate data cache in range
void CleanAndInvalidateDataCacheRange(u64 addr, u64 len)
{
	u64 line = addr & (~(DATA_CACHE_LINE_LENGTH - 1));
	len += addr - line;
	addr = line;
	while (True)
	{
		__asm volatile (" dc civac,%0\n" : : "r" (addr) : "memory");

		if (len <= DATA_CACHE_LINE_LENGTH) break;

		addr += DATA_CACHE_LINE_LENGTH;
		len  -= DATA_CACHE_LINE_LENGTH;
	}

	dsb();
}

#endif // AARCH==64
