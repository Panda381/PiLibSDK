PiLibSDK - Raspberry Pi bare-metal SDK library
==============================================
Pre-alpha version 0.10, in progress - under development

Copyright (c) 2026 Miroslav Nemecek

Panda38@seznam.cz
hardyplotter2@gmail.com
https://github.com/Panda381/PiLibSDK
https://www.breatharian.eu/hw/pilibsdk/index_en.html


License
=======
All of my source code and data are completely free to use for any purpose. The
exception is certain files derived from third-party sources—these are subject
to the original author’s license. This includes most fonts, as well as portions
of source files taken from the Circle library and Linux code - these sections
are marked in the source files.

Most fonts in this library are not my creation. They were downloaded from the
internet from unknown sources, with unknown license terms of use. If you want
to use only strict licenses, do not use fonts from this library.


Compilation
===========
To compile 32-bit mode, you will need AArch32 GCC
(tool name "arm-none-eabi"). Install to C:\ARM_GCC32.
https://developer.arm.com/-/media/Files/downloads/gnu/15.2.rel1/binrel/arm-gnu-toolchain-15.2.rel1-mingw-w64-i686-arm-none-eabi.msi

To compile 64-bit mode, you will need AArch64 GCC
(tool name "aarch64-none-elf"). Install to C:\ARM_GCC64.
 https://developer.arm.com/-/media/Files/downloads/gnu/15.2.rel1/binrel/arm-gnu-toolchain-15.2.rel1-mingw-w64-i686-aarch64-none-elf.msi
from https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads

If you will use another paths, edit paths in _c1.bat (set GCC_PI_PATH).


Directories
===========
!ZeroPC_Zero2_64 ... content of SD card for device "ZeroPC" with module Zero 2 64-bit.
_devices ... devices: pibase = base Pi module, zeroPC = ZeroPC module
_font ... fonts
_lib ... libraries
_sdk ... SDK (peripherals drivers)
_tools ... compilation tools
ZeroPC ... source codes of samples for ZeroPC device




Notes:

Raspberry Zero 2 W:
- PAM2306AYPKE dual PWM step-down DC-DC converter, input 2.5 to 5V, output 3.3V and 1.8V, current 1A
- RT8088AWSC 2.7MHz 3A Step-Down Converter with I2C Interface
- RP3A0 SiP (BCM2710A1 + 512 MB SDRAM LPDDR2), quartz 19.2 MHz
- RT9081A-1BGQZA(2) Richtek LDO Voltage Regulators 500mA 5.5V, input 3.3V, output DDR 1.25V


UART

To connect Pi to PC via serial port, you need convertor
to USB port. Good choise is USB-serial adaptor PL2303TA.

   How to connect UART to Pi:

                    +---------+
                3V3 |.1     2.| 5V
              GPIO2 |.3     4.| 5V
              GPIO3 |.5     6.| GND  ----> connect to (black) GND of PL2303
              GPIO4 |.7     8.| GPIO14 - TXD ----> connect to (white) RxD of PL2303
                GND |.9    10.| GPIO15 - RXD
                      ......

	Do not connect +5V (red) from PL2303
	Do not connect TXD from PL2303 (green), it can have 5V level.

   You need console program (as PuTTY) to see demo text.
   Setup port to 115200 Baud, 8 bits, 1 stop bit, no parity,
   flow control None.

   In Device Manager, USB adaptor can be found under name:
           "Prolific USB-to-Serial Comm Port (COM10)".
