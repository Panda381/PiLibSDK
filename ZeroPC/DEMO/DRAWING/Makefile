
##############################################################################
#                      Architecture configuration
##############################################################################

# RUNMODE = 1 to 7 ... select PI model and 32/64-bit
#  RUNMODE=1: models Zero1/Pi1, 32-bit ............. KERNEL1.IMG, SoC BCM2835 (RASPPI=1), CPU ARM1176JZF-S (Armv6), 32-bit (AARCH=32), 1 core, 700 MHz, GPU VideoCore IV 250 MHz
#  RUNMODE=2: models Pi2_v1.1, 32-bit .............. KERNEL2.IMG, SoC BCM2836 (RASPPI=2), CPU Cortex-A7 (Armv7), 32-bit (AARCH=32), 4 cores, 900 MHz, GPU VideoCore IV 250 MHz
#  RUNMODE=3: models Zero2/Pi2_v1.2/Pi3, 32-bit .... KERNEL3.IMG, SoC BCM2837/BCM2710A1 (RASPPI=3), CPU Cortex-A53 (Armv8 64-bit or Armv7 32-bit ), 32-bit (AARCH=32), 4 cores, 1.0 or 1.2 GHz, GPU VideoCore IV 400 MHz
#  RUNMODE=4: models Zero2/Pi2_v1.2/Pi3, 64-bit .... KERNEL4.IMG, SoC BCM2837/BCM2710A1 (RASPPI=3), CPU Cortex-A53 (Armv8 64-bit or Armv7 32-bit ), 64-bit (AARCH=64), 4 cores, 1.0 or 1.2 GHz, GPU VideoCore IV 400 MHz
#  RUNMODE=5: models Pi4, 32-bit ................... KERNEL5.IMG, SoC BCM2711 (RASPPI=4), CPU Cortex-A72 (Armv8-A 64-bit or Armv7-LPAE 32-bit), 32-bit (AARCH=32), 4 cores, 1.5 GHz, GPU VideoCore VI 3D 500 MHz
#  RUNMODE=6: models Pi4, 64-bit ................... KERNEL6.IMG, SoC BCM2711 (RASPPI=4), CPU Cortex-A72 (Armv8-A 64-bit or Armv7-LPAE 32-bit), 64-bit (AARCH=64), 4 cores, 1.5 GHz, GPU VideoCore VI 3D 500 MHz
#  RUNMODE=7: models Pi5, 64-bit ................... KERNEL7.IMG, SoC BCM2712 (RASPPI=5), CPU Cortex-A76 (Armv8-A), 64-bit (AARCH=64), 4 cores, 2.4 GHz, GPU VideoCore V3D VII 960 MHz

# ... This flag is taken from *.BAT via system variables
# RUNMODE=3	# 1=Zero1, 3=Zero2 32-bit, 4=Zero2 64-bit

# Setup device class (pibase = default board)
DEVCLASS=ZeroPC

# Path to root directory from the project directory (without trailing '/' delimiter)
PI_ROOT_PATH=../../..

# Makefile includes
include ${PI_ROOT_PATH}/Makefile1.mk

##############################################################################
#                           Project base configuration
##############################################################################

# Target filename (without extension)
# ... Using default filename for the loader
TARGET=DRAWING

# 1=do not include boot loader
# 0=application with boot loader
NOBOOT=1

##############################################################################
#                             Input files
##############################################################################

# ASM source files
ASRC +=

# C source files
CSRC += img/test1.c
CSRC += img/test2.c
CSRC += img/test3.c
CSRC += snd/fanfare.c

# C++ source files
SRC += src/main.cpp

##############################################################################
#                         Library configuration
##############################################################################
# For available options see config_def.mk in root directory.

##############################################################################
#                         SDK configuration
##############################################################################
# For available options see config_def.mk in root directory.

##############################################################################
#                  Include build Makefile 2nd stage - Build
##############################################################################

# Makefile includes
include ${PI_ROOT_PATH}/Makefile2.mk
