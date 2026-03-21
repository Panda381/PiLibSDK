@echo off
rem Delete...

rem RUNMODE = 1 to 7 ... select PI model and 32/64-bit
rem  RUNMODE=1: models Zero1/Pi1, 32-bit ............. KERNEL1.IMG, SoC BCM2835 (RASPPI=1), CPU ARM1176JZF-S (Armv6), 32-bit (AARCH=32), 1 core, 700 MHz, GPU VideoCore IV 250 MHz
rem  RUNMODE=2: models Pi2_v1.1, 32-bit .............. KERNEL2.IMG, SoC BCM2836 (RASPPI=2), CPU Cortex-A7 (Armv7), 32-bit (AARCH=32), 4 cores, 900 MHz, GPU VideoCore IV 250 MHz
rem  RUNMODE=3: models Zero2/Pi2_v1.2/Pi3, 32-bit .... KERNEL3.IMG, SoC BCM2837/BCM2710A1 (RASPPI=3), CPU Cortex-A53 (Armv8 64-bit or Armv7 32-bit ), 32-bit (AARCH=32), 4 cores, 1.0 or 1.2 GHz, GPU VideoCore IV 400 MHz
rem  RUNMODE=4: models Zero2/Pi2_v1.2/Pi3, 64-bit .... KERNEL4.IMG, SoC BCM2837/BCM2710A1 (RASPPI=3), CPU Cortex-A53 (Armv8 64-bit or Armv7 32-bit ), 64-bit (AARCH=64), 4 cores, 1.0 or 1.2 GHz, GPU VideoCore IV 400 MHz
rem  RUNMODE=5: models Pi4, 32-bit ................... KERNEL5.IMG, SoC BCM2711 (RASPPI=4), CPU Cortex-A72 (Armv8-A 64-bit or Armv7-LPAE 32-bit), 32-bit (AARCH=32), 4 cores, 1.5 GHz, GPU VideoCore VI 3D 500 MHz
rem  RUNMODE=6: models Pi4, 64-bit ................... KERNEL6.IMG, SoC BCM2711 (RASPPI=4), CPU Cortex-A72 (Armv8-A 64-bit or Armv7-LPAE 32-bit), 64-bit (AARCH=64), 4 cores, 1.5 GHz, GPU VideoCore VI 3D 500 MHz
rem  RUNMODE=7: models Pi5, 64-bit ................... KERNEL7.IMG, SoC BCM2712 (RASPPI=5), CPU Cortex-A76 (Armv8-A), 64-bit (AARCH=64), 4 cores, 2.4 GHz, GPU VideoCore V3D VII 960 MHz

set RUNMODE=4

call ..\..\..\_d1.bat
