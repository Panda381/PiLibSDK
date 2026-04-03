
DEFINE += -D USE_BIGINT=$(USE_BIGINT)
DEFINE += -D USE_CALENDAR=$(USE_CALENDAR)
DEFINE += -D USE_CALENDAR64=$(USE_CALENDAR64)
DEFINE += -D USE_CRC=$(USE_CRC)
DEFINE += -D USE_DECNUM=$(USE_DECNUM)
DEFINE += -D USE_DRAW=$(USE_DRAW)
DEFINE += -D USE_FAT=$(USE_FAT)
DEFINE += -D USE_JPG=$(USE_JPG)
DEFINE += -D USE_LIST=$(USE_LIST)
DEFINE += -D USE_MALLOC=$(USE_MALLOC)
DEFINE += -D USE_MP3=$(USE_MP3)
DEFINE += -D USE_PNG=$(USE_PNG)
DEFINE += -D USE_PRINT=$(USE_PRINT)
DEFINE += -D USE_PWMSND=$(USE_PWMSND)
DEFINE += -D USE_RAND=$(USE_RAND)
DEFINE += -D USE_REAL=$(USE_REAL)
DEFINE += -D USE_STREAM=$(USE_STREAM)
DEFINE += -D USE_TEXT=$(USE_TEXT)
DEFINE += -D USE_TREE=$(USE_TREE)

ifeq (${OS},Windows_NT)
SRC += ${PI_LIB_DIR}/lib.cpp
else

ifeq ($(strip $(USE_BIGINT)),1)
SRC += ${PI_LIB_DIR}/bigint/bigint.cpp
SRC += ${PI_LIB_DIR}/bigint/bernoulli.cpp
endif
ifeq ($(strip $(USE_CALENDAR)),1)
SRC += ${PI_LIB_DIR}/src/lib_calendar.cpp
endif
ifeq ($(strip $(USE_CALENDAR64)),1)
SRC += ${PI_LIB_DIR}/src/lib_calendar64.cpp
endif
ifeq ($(strip $(USE_CRC)),1)
SRC += ${PI_LIB_DIR}/src/lib_crc.cpp
endif
ifeq ($(strip $(USE_DECNUM)),1)
SRC += ${PI_LIB_DIR}/src/lib_decnum.cpp
endif
ifeq ($(strip $(USE_DRAW)),1)
SRC += ${PI_LIB_DIR}/src/lib_draw.cpp
endif
ifeq ($(strip $(USE_FAT)),1)
SRC += ${PI_LIB_DIR}/src/lib_fat.cpp
endif
ifeq ($(strip $(USE_JPG)),1)
SRC += ${PI_LIB_DIR}/pic/lib_jpg.cpp
endif
ifeq ($(strip $(USE_LIST)),1)
SRC += ${PI_LIB_DIR}/src/lib_list.cpp
endif
ifeq ($(strip $(USE_MALLOC)),1)
SRC += ${PI_LIB_DIR}/src/lib_malloc.cpp
endif
ifeq ($(strip $(USE_MP3)),1)
SRC += ${PI_LIB_DIR}/mp3/lib_mp3.cpp
endif
ifeq ($(strip $(USE_PNG)),1)
SRC += ${PI_LIB_DIR}/pic/lib_png.cpp
endif
ifeq ($(strip $(USE_PRINT)),1)
SRC += ${PI_LIB_DIR}/src/lib_print.cpp
endif
ifeq ($(strip $(USE_PWMSND)),1)
SRC += ${PI_LIB_DIR}/src/lib_pwmsnd.cpp
endif
ifeq ($(strip $(USE_RAND)),1)
SRC += ${PI_LIB_DIR}/src/lib_rand.cpp
endif
ifeq ($(strip $(USE_REAL)),1)
SRC += ${PI_LIB_DIR}/real/real.cpp
endif
ifeq ($(strip $(USE_STREAM)),1)
SRC += ${PI_LIB_DIR}/src/lib_stream.cpp
endif
ifeq ($(strip $(USE_TEXT)),1)
SRC += ${PI_LIB_DIR}/src/lib_text.cpp
SRC += ${PI_LIB_DIR}/src/lib_textlist.cpp
SRC += ${PI_LIB_DIR}/src/lib_textnum.cpp
endif
ifeq ($(strip $(USE_TREE)),1)
SRC += ${PI_LIB_DIR}/src/lib_tree.cpp
endif

endif
