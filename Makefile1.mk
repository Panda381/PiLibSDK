
# Setup from main Makefile
# ------------------------
# RUNMODE = 1 to 7 ... select PI model and 32/64-bit
#  RUNMODE=1: models Zero1/Pi1, 32-bit ............. KERNEL1.IMG, SoC BCM2835 (RASPPI=1), CPU ARM1176JZF-S (Armv6), 32-bit (AARCH=32), 1 core, 700 MHz, GPU VideoCore IV 250 MHz
#  RUNMODE=2: models Pi2_v1.1, 32-bit .............. KERNEL2.IMG, SoC BCM2836 (RASPPI=2), CPU Cortex-A7 (Armv7), 32-bit (AARCH=32), 4 cores, 900 MHz, GPU VideoCore IV 250 MHz
#  RUNMODE=3: models Zero2/Pi2_v1.2/Pi3, 32-bit .... KERNEL3.IMG, SoC BCM2837/BCM2710A1 (RASPPI=3), CPU Cortex-A53 (Armv8 64-bit or Armv7 32-bit ), 32-bit (AARCH=32), 4 cores, 1.0 or 1.2 GHz, GPU VideoCore IV 400 MHz
#  RUNMODE=4: models Zero2/Pi2_v1.2/Pi3, 64-bit .... KERNEL4.IMG, SoC BCM2837/BCM2710A1 (RASPPI=3), CPU Cortex-A53 (Armv8 64-bit or Armv7 32-bit ), 64-bit (AARCH=64), 4 cores, 1.0 or 1.2 GHz, GPU VideoCore IV 400 MHz
#  RUNMODE=5: models Pi4, 32-bit ................... KERNEL5.IMG, SoC BCM2711 (RASPPI=4), CPU Cortex-A72 (Armv8-A 64-bit or Armv7-LPAE 32-bit), 32-bit (AARCH=32), 4 cores, 1.5 GHz, GPU VideoCore VI 3D 500 MHz
#  RUNMODE=6: models Pi4, 64-bit ................... KERNEL6.IMG, SoC BCM2711 (RASPPI=4), CPU Cortex-A72 (Armv8-A 64-bit or Armv7-LPAE 32-bit), 64-bit (AARCH=64), 4 cores, 1.5 GHz, GPU VideoCore VI 3D 500 MHz
#  RUNMODE=7: models Pi5, 64-bit ................... KERNEL7.IMG, SoC BCM2712 (RASPPI=5), CPU Cortex-A76 (Armv8-A), 64-bit (AARCH=64), 4 cores, 2.4 GHz, GPU VideoCore V3D VII 960 MHz

# DEVCLASS=base .... Setup device class (base = default board)

# PI_ROOT_PATH=../.. ... Path to root directory from the project directory (without trailing '/' delimiter)

##############################################################################
#             Include Makefile default configuration
##############################################################################

include ${PI_ROOT_PATH}/config_def.mk

##############################################################################
#                       Makefile 1st stage - Setup
##############################################################################

# Path to _devices directory (without trailing '/' delimiter)
PI_DEVICES_DIR = ${PI_ROOT_PATH}/_devices

# Include device
ifneq ($(strip $(DEVCLASS)),)
include ${PI_DEVICES_DIR}/${DEVCLASS}/_makefile.inc
endif

# RASPPI = 1..5 ... SoC chip
#   RASPPI=1: SoC BCM2835 (models Zero1/Pi1)
#   RASPPI=2: SoC BCM2836 (models Pi2_v1.1)
#   RASPPI=3: SoC BCM2837/BCM2710A1 (models Zero2/Pi2_v1.2/Pi3)
#   RASPPI=4: SoC BCM2711 (models Pi4)
#   RASPPI=5: SoC BCM2712 (models Pi5)

# models Zero1/Pi1, 32-bit
ifeq ($(strip $(RUNMODE)),1)
ARCHCFG = -mcpu=arm1176jzf-s -marm -mfpu=vfp -mfloat-abi=hard
RASPPI=1
AARCH=32
CORES=1
else

# models Pi2_v1.1, 32-bit
ifeq ($(strip $(RUNMODE)),2)
ARCHCFG = -mcpu=cortex-a7 -marm -mfpu=neon-vfpv4 -mfloat-abi=hard
RASPPI=2
AARCH=32
CORES=4
else

# models Zero2/Pi2_v1.2/Pi3, 32-bit
ifeq ($(strip $(RUNMODE)),3)
ARCHCFG = -mcpu=cortex-a53 -marm -mfpu=neon-fp-armv8 -mfloat-abi=hard
RASPPI=3
AARCH=32
CORES=4
else

# models Zero2/Pi2_v1.2/Pi3, 64-bit
ifeq ($(strip $(RUNMODE)),4)
ARCHCFG = -mcpu=cortex-a53 -mlittle-endian
RASPPI=3
AARCH=64
CORES=4
else

# models Pi4
ifeq ($(strip $(RUNMODE)),5)
ARCHCFG = -mcpu=cortex-a72 -marm -mfpu=neon-fp-armv8 -mfloat-abi=hard
RASPPI=4
AARCH=32
CORES=4
else

# models Pi4
ifeq ($(strip $(RUNMODE)),6)
ARCHCFG = -mcpu=cortex-a72 -mlittle-endian
RASPPI=4
AARCH=64
CORES=4
else

# models Pi5
ifeq ($(strip $(RUNMODE)),7)
ARCHCFG = -mcpu=cortex-a76 -mlittle-endian
RASPPI=5
AARCH=64
CORES=4
else

# Default: Zero2, 64-bit
RUNMODE=4
ARCHCFG = -mcpu=cortex-a53 -mlittle-endian
RASPPI=3
AARCH=64
CORES=4

endif
endif
endif
endif
endif
endif
endif

# Common setup (1 stack per very core)

# 32/64-bit mode
ifeq ($(strip $(AARCH)),32)
SYS_STACK_SIZE=0x10000		# kernel stack size
INT_STACK_SIZE=0x4000		# interrupt stack size (used for ABORT, IRQ and FIQ)
CORE_STACK_SIZE=0x1C000		# stack size per 1 core (= 0x10000 + 3*0x4000 = SYS + 3*INT)
PREFIX=arm-none-eabi-
LOAD_ADDR=0x8000		# load address
else
INT_STACK_SIZE=0x8000		# interrupt stack size (used for ABORT, IRQ and FIQ)
SYS_STACK_SIZE=0x20000		# kernel stack size
CORE_STACK_SIZE=0x28000		# stack size per 1 core (= 0x8000 + 0x20000 = INT + SYS)
PREFIX=aarch64-none-elf-
LOAD_ADDR=0x80000		# load address
endif
