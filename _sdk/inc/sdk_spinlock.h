
// ****************************************************************************
//
//                              SpinLock
//
// ****************************************************************************
// SpinLock lock only locks global IRQ interrupts, but not fast FIQ interrupts.
// Therefore, FIQ interrupts must not contain lockable accesses to resources
// (such as the memory allocator).

// To lock spinlock with IRQ (not FIQ), use:
//   volatile u8 lock = 0; // global variable with the lock
//
//   IRQ_LOCK;
//   SpinLockLock(&lock);
//   ....
//   SpinLockUnlock(&lock);
//   IRQ_UNLOCK;

#ifndef _SDK_SPINLOCK_H
#define _SDK_SPINLOCK_H

#ifdef __cplusplus
extern "C" {
#endif

// initialize spinlock (1=locked, 0=unlocked)
void SpinLockInit(volatile u8* addr);

// try to lock spinlock (returns False if cannot lock, already locked)
Bool SpinLockTryLock(volatile u8* addr);

// lock spinlock
void SpinLockLock(volatile u8* addr);

// unlock spinlock
void SpinLockUnlock(volatile u8* addr);

#ifdef __cplusplus
}
#endif

#endif // _SDK_SPINLOCK_H
