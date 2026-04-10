PiLibSDK - Raspberry Pi bare-metal SDK library
==============================================
Pre-alpha version 0.20, in progress - under development
Last update: 04/10/2026


Copyright (c) 2026 Miroslav Nemecek

Panda38@seznam.cz
hardyplotter2@gmail.com
https://github.com/Panda381/PiLibSDK
https://www.breatharian.eu/hw/pilibsdk/index_en.html


Intro
=====
PiLibSDK is a bare-metal library for Raspberry Pi modules. "Bare-metal" means
that programs run directly on the hardware without an operating system, they
are not controlled by the operating system, and they have full access to the
hardware. Raspberry Pi modules can thus be used in a similar way to microchips.
Another advantage is that the device is fully operational within 3 seconds of
powering on. Currently, the Raspberry Zero 1, Zero 2 W, Pi 2, and Pi 3 modules
are supported. Support for the Pi 4 and Pi 5 modules is very limited, and they
will likely not be supported in the future either, as their benefits for
bare-metal use are minimal.

A boot loader is available for ZeroPC device, which allows easy launching of
programs from an SD card. The programs are ready for ZeroPC and ZeroTiny, with
Zero 1, Zero 2 32-bit, or Zero 2 64-bit modules. The Zero 1 and Zero 2 modules
differ primarily in speed - the Zero 2 is noticeably faster than the Zero 1.
The higher speed is due not only to a higher processor frequency but also to a
newer architecture. In addition, the Zero 2 module has 4 cores, while the
Zero 1 has only one core. Therefore, it is strongly recommended to use the
Zero 2 module. Use the Zero 1 module only if you own it and have no better use
for it. The Zero 2 module can be operated in 32-bit or 64-bit mode. The
compilations are prepared for both modes. From a user’s point of view, there is
no noticeable difference between the modes. It is recommended to prefer 64-bit
mode, which may be slightly faster in some cases.


License
=======
All of my source code and data are completely free to use for any purpose. The
exception is certain files derived from third-party sources - these are subject
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
!ZeroPC_1 ... content of SD card for device ZeroPC with module Zero 1
!ZeroPC_3 ... content of SD card for device ZeroPC with module Zero 2 32-bit
!ZeroPC_4 ... content of SD card for device ZeroPC with module Zero 2 64-bit
!ZeroTiny_1 ... content of SD card for device ZeroTiny with module Zero 1
!ZeroTiny_3 ... content of SD card for device ZeroTiny with module Zero 2 in 32-bit mode
!ZeroTiny_4 ... content of SD card for device ZeroTiny with module Zero 2 in 64-bit mode
_devices ... devices: PiBase = base Pi module, ZeroPC = ZeroPC device, ZeroTiny = ZeroTiny device
_font ... fonts
_lib ... libraries
_sdk ... SDK (peripherals drivers)
_tools ... compilation tools
Apps ... source codes of sample applications


ZeroTiny
========
ZeroTiny is the simplest game console based on the Raspberry Pi Zero 1 or
Zero 2 modules. It is recommended to preferably use the Zero 2 module in 64-bit
mode. The Zero 1 module may be noticeably slower in some applications. ZeroTiny
features 8 buttons, a stereo PWM audio output, and an HDMI display output.
A boot loader is available, allowing for easy program launch from an SD card.

The ZeroTiny is designed in a sandwich-style construction. In my prototype,
I connected the Zero module to the motherboard via an 8mm header - primarily to
facilitate easy module replacement. If you solder the Zero module to the board
using only a pin header, the design will be lower. However, this may make
repairs more difficult, such as replacing a button.

Detailed materials for ZeroPC hardware can be found in the
"_devices/ZeroTiny/diagram" folder.

Source code and sample programs can be found in the "Apps" folder.

You can find the compiled sample programs in the "!ZeroTiny_*" folders,
organized by module type and processor mode.


ZeroPC
======
ZeroPC is a small computer based on the Raspberry Pi Zero 1 or Zero 2 modules.
It is typically intended for use with the Zero 2 module in 64-bit mode. In
addition to the module, the ZeroPC includes stereo PWM audio output,
HDMI display output, an external connector for peripherals, and an internal
keyboard consisting of 49 microswitches. A boot loader is available, allowing
for easy program launching from an SD card.

I recommend not soldering the Zero module directly to the motherboard; instead,
use an 8mm slot - this will allow for easy replacement of the module if you
want to use it in another device or if you want to use a different module, such
as swapping the Zero 2 for a Zero 1.

Detailed materials for ZeroPC hardware can be found in the
"_devices/ZeroPC/diagram" folder.

Source code and sample programs can be found in the "Apps" folder.

You can find the compiled sample programs in the "!ZeroPC_*" folders, organized
by module type and processor mode.


Sample Applications
===================
System
 KERNEL ... Loader of applications
 SYSINFO ... System information

Book
 ABC ... Fairy Tales from the Alphabet (Czech)
 GINGER ... Gingerbread House (English)
 GINGERCZ ... Gingerbread House (Czech)

Demo
 BALLOONS ... Flying balloons
 BIGFACT ... Factorial of 123456789!
 DRAW ... Drawing graphic elements
 EARTH ... Rotating globe
 FLAG ... Fluttering flag
 FLAG2 ... Fluttering custom flag
 FOUNTAIN ... Draw 3D graph
 FRUITY .... 130 music loops with MP3 compression
 HELLO ... Simplest example
 HYPNO ... hypnotic rotating pattern
 LEVMETER ... simulation of music spectrum indicator
 LINEART ... draw line flower
 LINES ... relaxation line pattern generator
 MATRIX ... matrix code rain
 OSCIL ... simulation of oscilloscope signal 
 PF2027 ... Christmas animation
 PI ... calculating number Pi to 4780 digits
 PIXELS ... random generation of colored pixels
 RAYTRACE ... 3D pattern generation by ray tracing method
 SPHERES ... random spheres
 SPOTS ... random spots
 TWISTER ... twisting textured block
 WATER ... simulation of rippling water surface

MP3
 MP3 ... MP3 player


----------------

Notes:

Raspberry Zero 2 W contains:
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
