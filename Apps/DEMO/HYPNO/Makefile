
##############################################################################
#                      Architecture configuration
##############################################################################

# Path to root directory from the project directory (without trailing '/' delimiter)
PI_ROOT_PATH=../../..

# Makefile includes
include ${PI_ROOT_PATH}/Makefile1.mk

##############################################################################
#                             Input files
##############################################################################

# ASM source files
ASRC +=

# C source files
CSRC +=

# C++ source files
SRC += src/main.cpp

##############################################################################
#                         Library configuration
##############################################################################
# For available options see config_def.mk in root directory.

# 1=use BIT integers, 0=not used (_lib/bigint/*)
USE_BIGINT=0

# 1=use Calendar 32-bit (year range 1970..2099), 0=not used (lib_calendar.*)
USE_CALENDAR=0

# 1=use Calendar 64-bit (year range -29227..+29227), 0=not used (lib_calendar64.*)
USE_CALENDAR64=0

# 1=use CRC Cyclic Redundancy Check, 0=not used (lib_crc.*)
USE_CRC=0

# 1=use JPG support, 0=not used (_lib/pic/lib_jpg*)
USE_JPG=0

# 1=use 2D transformation matrix (lib_mat2d.*)
USE_MAT2D=1

# 1=use MP3 decoder, 0=not used (mp3/lib_mp3.*)
USE_MP3=0

# 1=use PNG support, 0=not used (_lib/pic/lib_png*)
USE_PNG=0

# 1=use PWM sound output, 0=not used (lib_pwmsnd.*)
USE_PWMSND=0

# 1=use REAL numbers, 0=not used (_lib/real/*)
USE_REAL=0

# 1=use text "string", 0=not used (lib_text*)
USE_TEXT=0

##############################################################################
#                         Font configuration
##############################################################################
# For available options see config_def.mk in root directory.

# 1=use support of proportional fonts, 0=not used
USE_FONTPROP=0

##############################################################################
#                         SDK configuration
##############################################################################
# For available options see config_def.mk in root directory.

##############################################################################
#                  Include build Makefile 2nd stage - Build
##############################################################################

# Makefile includes
include ${PI_ROOT_PATH}/Makefile2.mk
