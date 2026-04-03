
// ****************************************************************************
//
//                                Power Manager
//
// ****************************************************************************

#include "../../includes.h"	// includes

// watchdog start/restart
//  sec ... timeout in seconds 0..15
void WDG_Start(int sec)
{
	// setup watchdog timeout
	PWR->WDOG = ARM_PM_PASSWD | (sec << 16);

	// select full reboot mode
	PWR->RSTC = ARM_PM_PASSWD | ARM_PM_RSTC_REBOOT | (PWR->RSTC & ARM_PM_RSTC_CLEAR);
}

// watchdog stop
void WDG_Stop(void)
{
	// stop watchdog
	PWR->RSTC = ARM_PM_PASSWD | ARM_PM_RSTC_RESET;
}

// Restart from partition
//  part ... mask of forbidden partitions 0..5
//	ARM_PM_PART_ALL ... enable all partitions
//	ARM_PM_PART_HALT ... none partition, halt the system
void RestartFrom(int part)
{
	// prepare partition mask
	u32 rsts = 	(part & B0) |
			((part & B1) << 1) |
			((part & B2) << 2) |
			((part & B3) << 3) |
			((part & B4) << 4) |
			((part & B5) << 5);

	// select partitions
	PWR->RSTS = ARM_PM_PASSWD | rsts | (PWR->RSTS & ARM_PM_RSTS_PART_CLEAR);

	// select watchdog timeout - about 150us
	PWR->WDOG = ARM_PM_PASSWD | 10;

	// select full reboot mode
	PWR->RSTC = ARM_PM_PASSWD | ARM_PM_RSTC_REBOOT | (PWR->RSTC & ARM_PM_RSTC_CLEAR);

	// wait for reset
	for (;;) {}
}

// Reboot system
void Reboot(void)
{
	// select watchdog short timeout 15us
	PWR->WDOG = ARM_PM_PASSWD | 1;

	// do full reboot
	PWR->RSTC = ARM_PM_PASSWD | ARM_PM_RSTC_REBOOT;

	// wait for reset
	for (;;) {}
}
