PiLibSDK - Raspberry Pi bare-metal SDK library
==============================================
Pre-alpha version 0.46, in progress - under development
Last update: 08/05/2026


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

Sample programs have been prepared for the BarePi modular microchip kit.
(links: https://www.breatharian.eu/hw/barepi/index_en.html,
https://github.com/Panda381/BarePi). Of course, you don't have to follow the
hardware wiring exactly, but this is the most common pin-to-signal mapping.
The main device is the ZeroTiny game console, which corresponds to the
configuration of the "Zero," "Base," and "KeyPad" modules.

An LCD SPI display with a resolution of 320x240 pixels and ST7789 controller
can also be connected to the device. The wiring corresponds to the
"LCD320x240" module from the BarePi kit. The software library is designed so
that when it detects the connection of the LCD320x240 module, it activates the
SPI output to the display and sends the image to this LCD display as well.
Detection is performed using a 10K resistor connected between the DC and CS
pins. If you want to be able to disable the LCD display output on the target
device, you can do so using a switch that connects this 10K resistor. Detection
occurs when the application or loader starts - so after changing the switch,
you must terminate or start the program or loader. Disabling the output to the
LCD display ensures faster program execution, as sending data to the LCD
display heavily loads the program.

Although the LCD SPI display has a resolution of only 320x240 pixels, it is
possible to use programs with a resolution of 640x480 pixels. The driver
automatically scales the image down to the target size. If you need to display
details on the LCD display that would be lost due to the scaling, you can do so
by pressing Alt+[A] - the "Insert" function is used here to mean "Zoom". When
you press the keys, the LCD display will sequentially show each of the four
quadrants of the image in full resolution; pressing the keys a fifth time
restores the full scaled-down image.

If you do not need the HDMI output while using the LCD display, disconnect the
monitor cable. This will result in a significant reduction in current
consumption (from approximately 400 mA to 200 mA), which is especially
important when running the console on battery power. The system detects whether
an HDMI monitor is connected when the program starts - therefore, after
disconnecting or connecting an HDMI monitor, you must exit or restart the
program or loader. With the Raspberry Zero 1 module, it may be necessary to
disconnect and reconnect the power supply to detect the HDMI monitor.

If you connect a DS3231 real-time clock circuit to the device, applications
will retrieve the current real-time clock value from it. The I2C EEPROM memory
provides the ability to save program configurations.

A boot loader is available for BarePi devices, which allows easy launching of
programs from an SD card. The programs are ready for BarePi and ZeroTiny with
Zero 1, Zero 2 32-bit, or Zero 2 64-bit modules. The Zero 1 and Zero 2 modules
differ primarily in speed - the Zero 2 is noticeably faster than the Zero 1.
The higher speed is due not only to a higher processor frequency but also to a
newer architecture. In addition, the Zero 2 module has 4 cores, while the
Zero 1 has only one core. Therefore, it is strongly recommended to use the
Zero 2 module. Use the Zero 1 module only if you own it and have no better use
for it. The Zero 2 module can be operated in 32-bit or 64-bit mode. 64-bit mode
is faster. The compilations are prepared for both modes. From a user's point of
view, there is no noticeable difference between the modes. It is recommended to
prefer 64-bit mode, which may be slightly faster in some cases.

The BarePi kit can be controlled using either the "KeyPad" gaming keyboard or
the "MiniKey" alphanumeric keyboard. The keys on the "KeyPad" keyboard are also
mapped to the keys on the "MiniKey" keyboard. To distinguish between key
labels, action keys from the gaming keyboard are listed in square brackets,
while character keys are listed without brackets. For example, "[A]" denotes
the "A" action key on the gaming keyboard. The meanings of the keys on the
gaming keyboard and their mapping to the alphanumeric keyboard are as follows.

[A] = Space ... Main action button
[B] = Enter ... Secondary action button
[X] = Tab ... Auxiliary button, e.g. Help
[Y] = Esc ... Exit the menu, close the program
Alt+Left = Home ... Jump to start
Alt+Right = End ... Jump to end
Alt+Up = PgUp ... Page up
Alt+Down = PgDn ... Page down
Alt+A = Insert ... Insert mode (on LCD display, the "Zoom" function)
Alt+B = Edit ... Edit mode
Alt+X = PrtScr ... Take a screenshot and save it to an SD card
Alt+Y = Menu ... Call system menu

Note: A boot loader is not required to run programs. You can run programs
directly on the Zero1/Zero2 module itself, without additional hardware or a
boot loader. Copy the system files from the Root directory of the corresponding
!BarePi* folder to the SD card. You will need the files "bootcode.bin",
"config.txt", "fixup.dat", and "start.elf". Copy the program to the Root folder
as well, and rename it to KERNEL.IMG. The program will start automatically when
power is turned on.


License
=======
All of my source code and data are completely free to use for any purpose. The
exception is certain files derived from third-party sources - these are subject
to the original author's license. This includes most fonts, as well as portions
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
!BarePi ... content of SD card for kit BarePi and ZeroTiny - common files
!BarePi_1 ... content of SD card for kit BarePi and ZeroTiny - executables for module Zero 1
!BarePi_3 ... content of SD card for kit BarePi and ZeroTiny - executables for module Zero 2 32-bit
!BarePi_4 ... content of SD card for kit BarePi and ZeroTiny - executables for module Zero 2 64-bit
_devices ... devices: BarePi = modular microchip kit and ZeroTiny device
_drv ... drivers
_font ... fonts
_lib ... libraries
_sdk ... SDK (peripheral controllers)
_tools ... compilation tools
Apps ... source codes of sample applications

To prepare the SD card, format it to FAT32, copy the contents of the !BarePi
folder to it, as well as one of the !BarePi_* folder (depending on the module
type).

Order of includes:

-> project "include.h"
   -> root "includes.h" (INCLUDES_H)
       -> root "global.h"
          -> project "config.h"
             -> root "config_def.h" (CONFIG_DEF_H)
                -> (called from start of config_def.h) device "_devices/barepi/_config.h"
       -> fonts "_font/_include.h"
       -> SDK "_sdk/_include.h"
       -> drivers "_drv/_include.h"
       -> LIB "_lib/_include.h"
       -> device "_devices/barepi/_include.h"


ZeroTiny
========
ZeroTiny is the simplest game console based on the Raspberry Pi Zero 1 or
Zero 2 modules. This corresponds to the assembly of the "Zero," "Base," and
"KeyPad" modules from the BarePi kit. It is recommended to preferably use the
Zero 2 module in 64-bit mode. The Zero 1 module may be noticeably slower in
some applications. ZeroTiny features 9 buttons, a stereo PWM audio output, and
an HDMI display output. A boot loader is available, allowing for easy program
launch from an SD card.

The ZeroTiny is designed in a sandwich-style construction. In my prototype,
I connected the Zero module to the motherboard via an 8mm header - primarily to
facilitate easy module replacement. If you solder the Zero module to the board
using only a pin header, the design will be lower. However, this may make
repairs more difficult, such as replacing a button.

Detailed materials for ZeroTiny hardware can be found in the
"_devices/BarePi/ZeroTiny" folder.

Source code and sample programs can be found in the "Apps" folder.

You can find the compiled sample programs in the "!BarePi*" folders,
organized by module type and processor mode.


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

Games
 ANTS ... Card game
 ATOMS ... Game of exploding atoms
 EGGS ... Logic game as Reversi
 FIFTEEN ... Logic puzzle game
 FLAPPY ... Logic game from Sharp MZ800
 CHESS ... Chess game
 INVADERS ... Shooting game Space Invaders
 LIFE ... Cell life simulator
 MAZE ... Find way out of maze
 PACMAN ... Action game Pac-Man
 PICTOR ... Picopad Collector
 RAPTOR ... Shooting game
 SOKOBAN ... Logic game, 3008 levels
 TETRIS ... Game stacking blocks
 TRAIN ... Logic game, 50 levels
 TVTENNIS ... Classic TV game
 VEGASLOT ... Winning Slot Machine
 ZOOM ... Buck Rogers-Planet of Zoom

MP3
 MP3 ... MP3 player

Program
 MANDELBROT ... Fast fractal pattern generator

Test
 CALCKEY ... Test CalcKey keyboard
 EEPROM ... View contents of I2C EEPROMs
 I2CSCAN ... Scan I2C0 and I2C1 bus
 LCD ... Test LCD SPI display
 LCD16X2 ... Test LCD16x2 text display
 LED12 ... Test LED12 display
 MINIKEY ... Test MiniKey keyboard
 PORT12 ... Test PORT12 module
 RTC ... Display and set current time, using RTC module
 SSD1306 ... Test SSD1306 display
 TESTLED ... Test BarePi bus with TESTLED module

TinyGames
 TARKAN .. Tiny Arkanoid
 TBERT ... Tiny Bert
 TBIKE ... Tiny Bike
 TBOMBER ... Tiny Bomber
 TDDUG ... Tiny DDug
 TDOC ... Tiny Doctor
 TDUNGEON ... Tiny Dungeon
 TINVADER ... Tiny Invaders
 TMISSILE ... Tiny Missile
 TMORPION ... Tiny Morpion
 TPIPE ... Tiny Pipe
 TPLAQUE ... Tiny Plaque
 TSQUEST ... Tiny SQuest
 TTRICK ... Tiny Trick
 TTRIS ... Tiny Tris



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



Exporting a TGA image with transparency, using Photoshop:

- Select / Load Selection, Channel = Layer 0 Transparency (do not check Invert)
- Select / Save Selection, Channel = New (Name can remain blank, Alpha 1 will be used)
- Save As, switch to TGA, "Alpha Channels" must be checked, resolution 32 bits/pixel


Importing a TGA image with transparency, using Photoshop:

- Open and load TGA
- Layer / New / Layer from Background
- Select / Load Selection, Channel = Alpha 1 (do not check Invert)
- Layer / Layer Mask / Reveal Selection
- Layer / Layer Mask / Apply
- In Channels, select the Alpha 1 channel and cancel it
