
# Setup from main Makefile
# ------------------------
# AARCH = 32 or 64 .... use 32-bit or 64-bit architecture if available: RASPPI=3 or 4 (64-bit preferred)
# RASPPI = 1..5 ... Raspberry model class
# DEVCLASS=base .... Setup device class (base = default board)
# PI_ROOT_PATH=../.. ... Path to root directory from the project directory (without trailing '/' delimiter)
# TARGET = target filename (without extension)

##############################################################################
#                     Makefile 2nd stage - Build
##############################################################################

# Path to root directory from the project directory (without trailing '/' delimiter)
ifeq ($(PI_ROOT_PATH),)
PI_ROOT_PATH = ../../..
endif

# Path to _font directory (without trailing '/' delimiter)
ifeq ($(PI_FONT_DIR),)
PI_FONT_DIR = ${PI_ROOT_PATH}/_font
endif

# Path to _lib directory (without trailing '/' delimiter)
ifeq ($(PI_LIB_DIR),)
PI_LIB_DIR = ${PI_ROOT_PATH}/_lib
endif

# Path to _loader directory (without trailing '/' delimiter)
ifeq ($(PI_LOADER_DIR),)
PI_LOADER_DIR = ${PI_ROOT_PATH}/_loader
endif

# Path to _sdk directory (without trailing '/' delimiter)
ifeq ($(PI_SDK_DIR),)
PI_SDK_DIR = ${PI_ROOT_PATH}/_sdk
endif

# Path to _tools directory (without trailing '/' delimiter)
ifeq ($(PI_TOOLS_DIR),)
PI_TOOLS_DIR = ${PI_ROOT_PATH}/_tools
endif

##############################################################################
# Input files

# Do not include stdio.h file
DEFINE += -D _STDIO_H_

# SDK
include ${PI_SDK_DIR}/_makefile.inc

# Libraries
include ${PI_LIB_DIR}/_makefile.inc

# fonts
include ${PI_FONT_DIR}/_makefile.inc

# C code define include base "includes.h" file
DEFINE += -D INCLUDES_H=\"${PI_ROOT_PATH}/includes.h\"

# C code define include base "config_def.h" file
DEFINE += -D CONFIG_DEF_H=\"${PI_ROOT_PATH}/config_def.h\"

# Includes
IPATH += -I.

##############################################################################
# Configuration

# temporary build directory
TEMP = ./build

# Base makefile (to ensure recompilation after change)
MAKEFILE = ./Makefile

# Base include file (to ensure recompilation after change)
INCLUDEFILE = ./include.h

# Linker script
LDSCRIPT = ${PI_ROOT_PATH}/linker.ld

##############################################################################
# Passing configuration parameters to the source code

# SDK subdir
DEFINE += -D SDK_SUBDIR=${PI_SDK_DIR}

# Architecture 32/64 bits
DEFINE += -D AARCH=${AARCH}

# RASPPI
DEFINE += -D RASPPI=${RASPPI}

# Number of cores
DEFINE += -D CORES=${CORES}

# System stack size
DEFINE += -D SYS_STACK_SIZE=${SYS_STACK_SIZE}

# Interrupt stack size (used for ABORT, IRQ and FIQ)
DEFINE += -D INT_STACK_SIZE=${INT_STACK_SIZE}

# Core stack size
DEFINE += -D CORE_STACK_SIZE=${CORE_STACK_SIZE}

# Load address
DEFINE += -D LOAD_ADDR=${LOAD_ADDR}

# Page table size
DEFINE += -D PAGE_TABLE_SIZE=${PAGE_TABLE_SIZE}

##############################################################################
# Linker setup

# verbose - use this to see linker errors
#LDFLAGS += -Wl,-verbose

# Architecture
LDFLAGS += $(ARCHCFG)

# Entry point
LDFLAGS += --entry _start

# Section start
LDFLAGS += -Wl,--section-start=.init=$(LOAD_ADDR)

# No built-in functions (This is case when compiller changes printf("x") to putchar('x'))
LDFLAGS += -Wl,--build-id=none

# Link with libnosys.a (libc without system functions)
# LDFLAGS += -Wl,--specs=nano.specs

# Generate map file
LDFLAGS += -Wl,-Map=$(TARGET).map

# Linker script
LDFLAGS += -T $(LDSCRIPT)

# Eliminate unused sections
LDFLAGS += -Wl,--gc-sections

# No warnings "LOAD segment with RWX permissions"
LDFLAGS += -Wl,--no-warn-rwx-segment

# No start files
LDFLAGS += -nostartfiles

# wrap library functions
ifeq ($(USE_MALLOC),1)
LDFLAGS += -Wl,--wrap=malloc
LDFLAGS += -Wl,--wrap=free
LDFLAGS += -Wl,--wrap=realloc
endif

ifeq ($(USE_PRINT),1)
LDFLAGS += -Wl,--wrap=vsprintf
LDFLAGS += -Wl,--wrap=vsnprintf
LDFLAGS += -Wl,--wrap=sprintf
LDFLAGS += -Wl,--wrap=snprintf
LDFLAGS += -Wl,--wrap=putchar
LDFLAGS += -Wl,--wrap=puts
LDFLAGS += -Wl,--wrap=vprintf
LDFLAGS += -Wl,--wrap=printf
LDFLAGS += -Wl,--wrap=getchar
endif

# Linker script symbols
LDFLAGS += -Wl,--defsym=CORES=${CORES}
LDFLAGS += -Wl,--defsym=SYS_STACK_SIZE=${SYS_STACK_SIZE}
LDFLAGS += -Wl,--defsym=INT_STACK_SIZE=${INT_STACK_SIZE}
LDFLAGS += -Wl,--defsym=CORE_STACK_SIZE=${CORE_STACK_SIZE}
LDFLAGS += -Wl,--defsym=LOAD_ADDR=${LOAD_ADDR}

##############################################################################
# Common compiler setup

# Output dependency rule
ARCHCFG += -MMD

# Add phony target for each dependency
ARCHCFG += -MP

# Dependency file
ARCHCFG += -MF"$(@:%.o=%.d)"

# Target dependency generation
ARCHCFG += -MT"$(@)"

##############################################################################
# ASM setup

# Common compiler setup
AFLAGS += $(ARCHCFG)

# Default language is assembler
AFLAGS += -x assembler-with-cpp

##############################################################################
# C setup

# Common compiler setup
CFLAGS += $(ARCHCFG)

# Optimization level
#CFLAGS += -O0	# do no optimization
#CFLAGS += -O1	# optimize minimally
#CFLAGS += -O2	# optimize more
#CFLAGS += -O3	# optimize even more
CFLAGS += -Ofast # optimize for speed
#CFLAGS += -Og -g3 # optimize for debugger (use together with -g0..-g3, level of debugging information)
#CFLAGS += -Os	# optimize for size

# Generate uninitialized global variable (to check multiple declarations)
CFLAGS += -fno-common

# Warn if variable is not initialized
CFLAGS += -Wuninitialized

# Create separate sections (to enable eliminate unused sections)
CFLAGS += -ffunction-sections -fdata-sections 

# Use unsigned char (this is default for ARM cpu)
CFLAGS += -funsigned-char

# Report if function does not return value
CFLAGS += -Wreturn-type

# Warning as errors
CFLAGS += -Werror

# do not assume usual definitions
CFLAGS += -ffreestanding

# using coherent memory region
# Prohibiting non-aligned access to peripherals and shared memory with the GPU, as they do not allow it.
ifeq ($(strip $(AARCH)),32)
CFLAGS += -mno-unaligned-access
else
CFLAGS += -mstrict-align
endif

# ===== Disable optimizations. If some operations cause an error, try disabling some optimizations.

# Optimizations -fno-tree-vrp and -fno-tree-dominator-opts must be disabled in GCC v15.2
# - they caused an error in the calculation of the DateTime64UnpackAbsTime() function.

# --- Promotion of constants and rearrangement of expressions
# These optimizations may change the order of calculations, merge expressions, or remove
# intermediate results that are diagnostically visible but "unnecessary" to GCC.

# Value Range Propagation
CFLAGS += -fno-tree-vrp

CFLAGS += -fno-tree-forwprop

# Full Redundancy Elimination
CFLAGS += -fno-tree-fre

# Scalar Replacement of Aggregates
#CFLAGS += -fno-tree-sra

# Dead Code Elimination
CFLAGS += -fno-tree-dce

CFLAGS += -fno-tree-dominator-opts

# --- Optimizations that can break integer calculations
# These optimizations manipulate expressions of the type x / C and can
# generate erroneous instruction sequences if a specific pattern occurs.

# Partial Redundancy Elimination
CFLAGS += -fno-tree-pre

CFLAGS += -fno-split-wide-types

CFLAGS += -fno-tree-scev-cprop

CFLAGS += -fno-tree-dse

# --- Optimizations that can break instruction order and registry operations
# If GCC misestimates the lifetime of registers or dependencies between instructions,
# it may happen that the operands are correct but the result is wrong.

CFLAGS += -fno-schedule-insns

CFLAGS += -fno-schedule-insns2

CFLAGS += -fno-reorder-blocks

#CFLAGS += -fno-reorder-functions

#CFLAGS += -fno-cse-follow-jumps

#CFLAGS += -fno-cse-skip-blocks

# --- Optimizations that can break arithmetic due to aliasing or reordering

#CFLAGS += -fno-strict-aliasing

# Temporary Expression Replacement
CFLAGS += -fno-tree-ter

#CFLAGS += -fno-tree-coalesce-vars

# --- Optimizations that can break code in ARM bare-metal environments
# Interprocedural optimizations are a frequent source of errors
# if GCC misjudges the purity of a function or its side effects.

# Interprocedural Constant Propagation
CFLAGS += -fno-ipa-cp

CFLAGS += -fno-ipa-sra

#CFLAGS += -fno-ipa-pure-const

#CFLAGS += -fno-ipa-reference

# Register Allocation across functions
CFLAGS += -fno-ipa-ra

##############################################################################
# C++ setup

# Common compiler setup
CPPFLAGS += $(CFLAGS)

# Do not use exceptions and unwind frames
CPPFLAGS += -fno-exceptions -fno-unwind-tables

# CPPFLAGS += -fno-rtti -fno-use-cxa-atexit -std=gnu++17
CPPFLAGS += -std=c++14 -Wno-aligned-new -fno-rtti -nostdinc++

##############################################################################
# Compiler tools

# C compiler name
CC = ${PREFIX}gcc

# C++ compiler name
CPP = ${PREFIX}g++

# ASM compiler name
AS = ${PREFIX}gcc

# Linker
LD = ${PREFIX}g++

# Object dump
OD = ${PREFIX}objdump

# Object copy
OC = ${PREFIX}objcopy

# Names
NM = ${PREFIX}nm

# Size
SZ = ${PREFIX}size

##############################################################################
# File list

# List of all source files
ALLSRC = $(ASRC) $(CSRC) $(SRC)

# List of all files without directory and without extension
ALLBASE = $(notdir $(basename $(ALLSRC)))

# List of all object files *.o
ALLOBJ = $(addprefix $(TEMP)/, $(addsuffix .o, $(ALLBASE)))

ifeq ($(strip $(AARCH)),64)
CRTBEGIN = "$(shell $(CPP) -print-file-name=crtbegin.o)"
CRTEND   = "$(shell $(CPP) -print-file-name=crtend.o)"
endif

##############################################################################
# Clear temporary directory (use "make clean")

clean:
	@${PI_TOOLS_DIR}/rm -rf $(TEMP)
	@${PI_TOOLS_DIR}/rm -f ./$(TARGET).lst ./$(TARGET).map ./$(TARGET).sym ./$(TARGET).siz ./$(TARGET).elf

##############################################################################
# Compile all (use "make all")

all: createdirs elf IMG lst sym siz

##############################################################################
# Create temporary directory

createdirs:
ifeq (${OS},Windows_NT)
	@${PI_TOOLS_DIR}/mkdir.exe -p $(TEMP)
else
	@mkdir -p $(TEMP)
endif
	@echo Target: ${TARGET}

##############################################################################
# Dependencies of output files

elf: ./$(TARGET).elf
IMG: ./$(TARGET).IMG
lst: ./$(TARGET).lst
sym: ./$(TARGET).sym
siz: ./$(TARGET).siz

##############################################################################
# Create .IMG file from .elf file

./$(TARGET).IMG: ./$(TARGET).elf
	@echo     IMG		 $@
	@$(OC) -O binary $< $@

##############################################################################
# Create .lst file from .elf file

./$(TARGET).lst: ./$(TARGET).elf
	@echo     lst		 $@
	@$(OD) --all-headers -d -S $< > $@

##############################################################################
# Create .sym file from .elf file

./$(TARGET).sym: ./$(TARGET).elf
	@echo     sym		 $@
	@$(NM) -n $< > $@

##############################################################################
# Create .siz file from .elf file

./$(TARGET).siz: ./$(TARGET).elf
	@echo     siz		 $@
	@$(SZ) $< > $@
	@echo.
	@type $@

##############################################################################
# Create .o files from .c files

define CC_TEMP
$(TEMP)/$(notdir $(basename $(1))).o : $(1) $(MAKEFILE) $(INCLUDEFILE)
	@echo     CC		 $$<
	@$(CC) $$(DEFINE) $$(IPATH) $$(CFLAGS) -std=gnu11 -c $$< -o $$@
endef

$(foreach src, $(CSRC), $(eval $(call CC_TEMP, $(src))))

##############################################################################
# Create .o files from .cpp files

define CPP_TEMP
$(TEMP)/$(notdir $(basename $(1))).o : $(1) $(MAKEFILE) $(INCLUDEFILE)
	@echo     C++		 $$<
	@$(CPP) $$(DEFINE) $$(IPATH) $$(CPPFLAGS) -c $$< -o $$@
endef

$(foreach src, $(SRC), $(eval $(call CPP_TEMP, $(src))))

##############################################################################
# Create .o files from .S files

define AS_TEMP
$(TEMP)/$(notdir $(basename $(1))).o : $(1) $(MAKEFILE) $(INCLUDEFILE)
	@echo     ASM		 $$<
	@$(CC) $$(DEFINE) $$(IPATH) $$(AFLAGS) -c $$< -o $$@
endef

$(foreach src, $(ASRC), $(eval $(call AS_TEMP, $(src))))

##############################################################################
# Create .elf from .o files

./$(TARGET).elf: $(ALLOBJ) $(MAKEFILE) $(LDSCRIPT)
	@echo     ld		 $@
	@$(LD) $(LDFLAGS) $(CRTBEGIN) $(ALLOBJ) $(CRTEND) -o $@

##############################################################################
# Head dependencies, to recompile C after changing H

ifneq (${MAKECMDGOALS},clean)
-include $(wildcard $(TEMP)/*.d)
endif

##############################################################################
# List of dependencies not creating files

.PHONY: clean all createdirs elf IMG lst sym siz uf2
