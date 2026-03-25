
##############################################################################
#
#                     Makefile default configuration
#
##############################################################################

##############################################################################
#                         Library configuration
##############################################################################
# Select library modules (folder _lib)
# If you add a new flag, add the update flag to _lib\_makefile.inc.

# 1=use BIT integers, 0=not used (_lib/bigint/*)
USE_BIGINT=0

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

# 1=use REAL numbers, 0=not used (_lib/real/*)
USE_REAL=0

# 1=use Data stream, 0=not used (lib_stream.*)
USE_STREAM=1

# 1=use Tree list, 0=not used (lib_tree.*)
USE_TREE=1

##############################################################################
#                         Font configuration
##############################################################################
# Select fonts (folder _font)
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
# Select SDK modules (foldes _sdk)
# If you add a new flag, add the update flag to _sdk\_makefile.inc.
