
// ****************************************************************************
//
//                              SpinLock
//
// ****************************************************************************

#include "../../includes.h"	// includes

// initialize spinlock (1=locked, 0=unlocked)
void SpinLockInit(volatile u8* addr)
{
	*addr = 0;
}

// try to lock spinlock (returns False if cannot lock, already locked)
Bool SpinLockTryLock(volatile u8* addr)
{
	if (ExcLock8(addr, 1) == 0)
	{
		dmb_ish();
		return True;
	}
	return False;
}

// lock spinlock
void SpinLockLock(volatile u8* addr)
{
	while (ExcLock8(addr, 1) == 1) {}
	dmb_ish();
}

// unlock spinlock
void SpinLockUnlock(volatile u8* addr)
{
	dmb_ish();
	*addr = 0;
}
