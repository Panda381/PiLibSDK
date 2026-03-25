
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

# 1=use PWM sound output, 0=not used (lib_pwmsnd.*)
USE_PWMSND=0

# 1=use REAL numbers, 0=not used (_lib/real/*)
USE_REAL=0

##############################################################################
#                         Font configuration
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
