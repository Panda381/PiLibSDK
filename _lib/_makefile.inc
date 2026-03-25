
DEFINE += -D USE_BIGINT=$(USE_BIGINT)
DEFINE += -D USE_CALENDAR=$(USE_CALENDAR)
DEFINE += -D USE_CALENDAR64=$(USE_CALENDAR64)
DEFINE += -D USE_CRC=$(USE_CRC)
DEFINE += -D USE_DECNUM=$(USE_DECNUM)
DEFINE += -D USE_DRAW=$(USE_DRAW)
DEFINE += -D USE_FAT=$(USE_FAT)
DEFINE += -D USE_LIST=$(USE_LIST)
DEFINE += -D USE_MALLOC=$(USE_MALLOC)
DEFINE += -D USE_PRINT=$(USE_PRINT)
DEFINE += -D USE_PWMSND=$(USE_PWMSND)
DEFINE += -D USE_RAND=$(USE_RAND)
DEFINE += -D USE_REAL=$(USE_REAL)
DEFINE += -D USE_STREAM=$(USE_STREAM)
DEFINE += -D USE_TREE=$(USE_TREE)

ifeq (${OS},Windows_NT)
CSRC += ${PI_LIB_DIR}/lib.c
else

ifeq ($(strip $(USE_BIGINT)),1)
CSRC += ${PI_LIB_DIR}/bigint/bigint.c
CSRC += ${PI_LIB_DIR}/bigint/bernoulli.c
endif
ifeq ($(strip $(USE_CALENDAR)),1)
CSRC += ${PI_LIB_DIR}/src/lib_calendar.c
endif
ifeq ($(strip $(USE_CALENDAR64)),1)
CSRC += ${PI_LIB_DIR}/src/lib_calendar64.c
endif
ifeq ($(strip $(USE_CRC)),1)
CSRC += ${PI_LIB_DIR}/src/lib_crc.c
endif
ifeq ($(strip $(USE_DECNUM)),1)
CSRC += ${PI_LIB_DIR}/src/lib_decnum.c
endif
ifeq ($(strip $(USE_DRAW)),1)
CSRC += ${PI_LIB_DIR}/src/lib_draw.c
endif
ifeq ($(strip $(USE_FAT)),1)
CSRC += ${PI_LIB_DIR}/src/lib_fat.c
endif
ifeq ($(strip $(USE_LIST)),1)
CSRC += ${PI_LIB_DIR}/src/lib_list.c
endif
ifeq ($(strip $(USE_MALLOC)),1)
CSRC += ${PI_LIB_DIR}/src/lib_malloc.c
endif
ifeq ($(strip $(USE_PRINT)),1)
CSRC += ${PI_LIB_DIR}/src/lib_print.c
endif
ifeq ($(strip $(USE_PWMSND)),1)
CSRC += ${PI_LIB_DIR}/src/lib_pwmsnd.c
endif
ifeq ($(strip $(USE_RAND)),1)
CSRC += ${PI_LIB_DIR}/src/lib_rand.c
endif
ifeq ($(strip $(USE_REAL)),1)
CSRC += ${PI_LIB_DIR}/real/real.c
endif
ifeq ($(strip $(USE_STREAM)),1)
CSRC += ${PI_LIB_DIR}/src/lib_stream.c
endif
ifeq ($(strip $(USE_TREE)),1)
CSRC += ${PI_LIB_DIR}/src/lib_tree.c
endif

endif
