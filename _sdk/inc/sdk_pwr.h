
// ****************************************************************************
//
//                                Power Manager
//
// ****************************************************************************

#ifndef _SDK_PWR_H
#define _SDK_PWR_H

#ifdef __cplusplus
extern "C" {
#endif

// Power manager interface
typedef struct {
	io32	res[7];		// 0x00: ... reserved

	io32	RSTC;		// 0x1C: control register
				//	bit 4-5: watchdog reboot mode
				//			2 = do full reboot
				//	bit 24-31: set password 0x5A (ARM_PM_PASSWD) to accept

	io32	RSTS;		// 0x20: select register, which partition to boot from
				//  Disable all partitions (set to 1), to halt the system.
				//	bit 0: 0=enable boot from partition 0
				//	bit 2: 0=enable boot from partition 1
				//	bit 4: 0=enable boot from partition 2
				//	bit 6: 0=enable boot from partition 3
				//	bit 8: 0=enable boot from partition 4
				//	bit 10: 0=enable boot from partition 5
				//	bit 24-31: set password 0x5A (ARM_PM_PASSWD) to accept

	io32	WDOG;		// 0x24: watchdog register
				//	bit 0-15: watchdog timeout, fraction in 1/65536 sec (approx. multiply of 15us)
				//	bit 16-19: watchdog timeout, seconds 0..15
				//	bit 24-31: set password 0x5A (ARM_PM_PASSWD) to accept

	io32	res2;		// 0x28: ... reserved

	io32	PADS0;		// 0x2C: GPIO 0 - 27

	io32	PADS1;		// 0x30: GPIO 28 - 45

	io32	PADS2;		// 0x34: GPIO 46 - 53

} PWR_t;
STATIC_ASSERT(sizeof(PWR_t) == 0x38, "Incorrect PWR_t!");

// PWR interface
#define PWR ((PWR_t*)ARM_PM_BASE)

// constants
#define ARM_PM_PASSWD		(0x5A << 24)	// password
#define ARM_PM_RSTC_CLEAR	0xFFFFFFCF	// RSTC: clear reboot mode
#define ARM_PM_RSTC_REBOOT	0x00000020	// RSTC: reboot mode - do full reboot
#define ARM_PM_RSTC_RESET	0x00000102	// RSTC: stop watchdog
#define ARM_PM_RSTS_PART_CLEAR	0xFFFFFAAA	// RSTS: clear partitions
#define ARM_PM_PART_ALL		0		// RSTS: enable all partitions
#define ARM_PM_PART_HALT	0x3F		// RSTS: none partition, halt system

// watchdog start/restart
//  sec ... timeout in seconds 0..15
void WDG_Start(int sec);

// watchdog stop
void WDG_Stop(void);

// Restart from partition
//  part ... mask of forbidden partitions 0..5
//	ARM_PM_PART_ALL ... enable all partitions
//	ARM_PM_PART_HALT ... none partition, halt the system
void RestartFrom(int part);

// Reboot system
void Reboot(void);

#ifdef __cplusplus
}
#endif

#endif // _SDK_PWR_H
