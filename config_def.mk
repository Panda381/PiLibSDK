
##############################################################################
#
#                     Makefile default configuration
#
##############################################################################

##############################################################################
#                      Project base configuration
##############################################################################

# Default target filename (without extension)

# RUNMODE=1: models Zero1/Pi1, 32-bit ... KERNEL1.IMG, SoC BCM2835 (RASPPI=1), CPU ARM1176JZF-S (Armv6), 32-bit (AARCH=32), 1 core, 700 MHz, GPU VideoCore IV 250 MHz
ifeq ($(strip $(RUNMODE)),1)
TARGET=KERNEL1
else

# RUNMODE=2: models Pi2_v1.1, 32-bit ... KERNEL2.IMG, SoC BCM2836 (RASPPI=2), CPU Cortex-A7 (Armv7), 32-bit (AARCH=32), 4 cores, 900 MHz, GPU VideoCore IV 250 MHz
ifeq ($(strip $(RUNMODE)),2)
TARGET=KERNEL2
else

# RUNMODE=3: models Zero2/Pi2_v1.2/Pi3, 32-bit ... KERNEL3.IMG, SoC BCM2837/BCM2710A1 (RASPPI=3), CPU Cortex-A53 (Armv8 64-bit or Armv7 32-bit ), 32-bit (AARCH=32), 4 cores, 1.0 or 1.2 GHz, GPU VideoCore IV 400 MHz
ifeq ($(strip $(RUNMODE)),3)
TARGET=KERNEL3
else

# RUNMODE=4: models Zero2/Pi2_v1.2/Pi3, 64-bit ... KERNEL4.IMG, SoC BCM2837/BCM2710A1 (RASPPI=3), CPU Cortex-A53 (Armv8 64-bit or Armv7 32-bit ), 64-bit (AARCH=64), 4 cores, 1.0 or 1.2 GHz, GPU VideoCore IV 400 MHz
ifeq ($(strip $(RUNMODE)),4)
TARGET=KERNEL4
else

# RUNMODE=5: models Pi4, 32-bit ... KERNEL5.IMG, SoC BCM2711 (RASPPI=4), CPU Cortex-A72 (Armv8-A 64-bit or Armv7-LPAE 32-bit), 32-bit (AARCH=32), 4 cores, 1.5 GHz, GPU VideoCore VI 3D 500 MHz
ifeq ($(strip $(RUNMODE)),5)
TARGET=KERNEL5
else

# RUNMODE=6: models Pi4, 64-bit ... KERNEL6.IMG, SoC BCM2711 (RASPPI=4), CPU Cortex-A72 (Armv8-A 64-bit or Armv7-LPAE 32-bit), 64-bit (AARCH=64), 4 cores, 1.5 GHz, GPU VideoCore VI 3D 500 MHz
ifeq ($(strip $(RUNMODE)),6)
TARGET=KERNEL6
else

# RUNMODE=7: models Pi5, 64-bit ... KERNEL7.IMG, SoC BCM2712 (RASPPI=5), CPU Cortex-A76 (Armv8-A), 64-bit (AARCH=64), 4 cores, 2.4 GHz, GPU VideoCore V3D VII 960 MHz
ifeq ($(strip $(RUNMODE)),7)
TARGET=KERNEL7
else

endif
endif
endif
endif
endif
endif
endif

# 1=do not include boot loader
# 0=application with boot loader
NOBOOT=0

##############################################################################
#                         Library configuration
##############################################################################

# --- Select library modules (folder _lib)
# If you add a new flag, add the update flag to _lib\_makefile.inc.

# 1=use Calendar 32-bit (year range 1970..2099), 0=not used (lib_calendar.*)
USE_CALENDAR=1

# 1=use Calendar 64-bit (year range -29227..+29227), 0=not used (lib_calendar64.*)
USE_CALENDAR64=1

# 1=use CRC Cyclic Redundancy Check, 0=not used (lib_crc.*)
USE_CRC=1

# 1=use Decode integer number, 0=not used (lib_decnum.*)
USE_DECNUM=1

# 1=use Drawing library, 0=not used (lib_draw.*)
USE_DRAW=1

# 1=use FAT file system, 0=not used (lib_fat.*)
USE_FAT=1

# 1=use Doubly Linked List, 0=not used (lib_list.*)
USE_LIST=1

# 1=use memory allocator, 0=not used (lib_malloc.*)
USE_MALLOC=1

# 1=use Formatted print, 0=not used (lib_print.*)
USE_PRINT=1

# 1=use PWM sound output, 0=not used (lib_pwmsnd.*)
USE_PWMSND=1

# 1=use Random number generator, 0=not used (lib_rand.*)
USE_RAND=1

# 1=use Data stream, 0=not used (lib_stream.*)
USE_STREAM=1

# 1=use Tree list, 0=not used (lib_tree.*)
USE_TREE=1

# --- Select fonts (folder _font)
# If you add a new flag, add the update flag to _font\_makefile.inc.

# 1=include all fixed fonts, 0=include only default font FontBold8x16
USE_FONTFIX=1

# 1=use support of proportional fonts, 0=not used
USE_FONTPROP=1

# 1=include Arial fonts, 0=include only default font FontARB10
USE_FONTPROP_ARIAL=1

# 1=include Garamond fonts, 0=not included
USE_FONTPROP_GARAMOND=1

# 1=include Lucida Grande fonts, 0=not included
USE_FONTPROP_LUCIDA=1

# 1=include Sabon fonts, 0=not included
USE_FONTPROP_SABON=1

# 1=include Tahoma fonts, 0=not included
USE_FONTPROP_TAHOMA=1

# 1=include mixed fonts, 0=not included
USE_FONTPROP_MIXED=1

##############################################################################
#                         SDK configuration
##############################################################################
# If you add a new flag, add the update flag to _sdk\_makefile.inc.
