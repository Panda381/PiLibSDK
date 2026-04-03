
DEFINE += -D USE_FONTFIX=$(USE_FONTFIX)
DEFINE += -D USE_FONTPROP=$(USE_FONTPROP)
DEFINE += -D USE_FONTPROP_ARIAL=$(USE_FONTPROP_ARIAL)
DEFINE += -D USE_FONTPROP_GARAMOND=$(USE_FONTPROP_GARAMOND)
DEFINE += -D USE_FONTPROP_LUCIDA=$(USE_FONTPROP_LUCIDA)
DEFINE += -D USE_FONTPROP_SABON=$(USE_FONTPROP_SABON)
DEFINE += -D USE_FONTPROP_TAHOMA=$(USE_FONTPROP_TAHOMA)
DEFINE += -D USE_FONTPROP_MIXED=$(USE_FONTPROP_MIXED)

ifeq (${OS},Windows_NT)
SRC += ${PI_FONT_DIR}/font.cpp
else

SRC += ${PI_FONT_DIR}/fixed_cpp/font_bold_8x16.cpp
ifeq (${USE_FONTFIX},1)
SRC += ${PI_FONT_DIR}/fixed_cpp/font_bold_8x12.cpp
SRC += ${PI_FONT_DIR}/fixed_cpp/font_bold_8x14.cpp
SRC += ${PI_FONT_DIR}/fixed_cpp/font_bold_8x14B.cpp
SRC += ${PI_FONT_DIR}/fixed_cpp/font_bold_8x16B.cpp
SRC += ${PI_FONT_DIR}/fixed_cpp/font_bold_8x8.cpp
SRC += ${PI_FONT_DIR}/fixed_cpp/font_cond_6x6.cpp
SRC += ${PI_FONT_DIR}/fixed_cpp/font_cond_6x6B.cpp
SRC += ${PI_FONT_DIR}/fixed_cpp/font_cond_6x8.cpp
SRC += ${PI_FONT_DIR}/fixed_cpp/font_game_8x8.cpp
SRC += ${PI_FONT_DIR}/fixed_cpp/font_ibm_8x14.cpp
SRC += ${PI_FONT_DIR}/fixed_cpp/font_ibm_8x16.cpp
SRC += ${PI_FONT_DIR}/fixed_cpp/font_ibm_8x8.cpp
SRC += ${PI_FONT_DIR}/fixed_cpp/font_ibmtiny_8x8.cpp
SRC += ${PI_FONT_DIR}/fixed_cpp/font_italic_8x8.cpp
SRC += ${PI_FONT_DIR}/fixed_cpp/font_thin_8x8.cpp
SRC += ${PI_FONT_DIR}/fixed_cpp/font_tiny_5x8.cpp
SRC += ${PI_FONT_DIR}/fixed_cpp/font_zx.cpp
SRC += ${PI_FONT_DIR}/fixed_cpp/font_80.cpp
SRC += ${PI_FONT_DIR}/fixed_cpp/font_81.cpp
endif
ifeq (${USE_FONTPROP},1)
SRC += ${PI_FONT_DIR}/prop_cpp/font_arb10.cpp
ifeq (${USE_FONTPROP_ARIAL},1)
SRC += ${PI_FONT_DIR}/prop_cpp/font_ar08.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_ar09.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_ar10.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_ar11.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_ar12.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_ar14.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_arb08.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_arb09.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_arb11.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_arb12.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_arb14.cpp
endif
ifeq (${USE_FONTPROP_GARAMOND},1)
SRC += ${PI_FONT_DIR}/prop_cpp/font_gmb07.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_gmb08.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_gmb09.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_gmb10.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_gmb11.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_gmb12.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_gmb14.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_gmb16.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_gmb18.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_gmb20.cpp
endif
ifeq (${USE_FONTPROP_LUCIDA},1)
SRC += ${PI_FONT_DIR}/prop_cpp/font_lg08.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_lg09.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_lgb07.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_lgb08.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_lgb09.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_lgb10.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_lgb11.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_lgb12.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_lgb14.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_lgb16.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_lgb18.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_lgb20.cpp
endif
ifeq (${USE_FONTPROP_SABON},1)
SRC += ${PI_FONT_DIR}/prop_cpp/font_sb08.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_sb08x.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_sb09.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_sb09x.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_sb10.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_sb10x.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_sb11.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_sb11x.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_sb12.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_sb12x.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_sb14.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_sb14x.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_sb16.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_sb16x.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_sbr10.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_sbr11.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_sbr11x.cpp
endif
ifeq (${USE_FONTPROP_TAHOMA},1)
SRC += ${PI_FONT_DIR}/prop_cpp/font_tm07.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_tm08.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_tm09.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_tm10.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_tmb07.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_tmb08.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_tmb09.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_tmb10.cpp
endif
ifeq (${USE_FONTPROP_MIXED},1)
SRC += ${PI_FONT_DIR}/prop_cpp/font_digital.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_klvkb36.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_mss08.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_sf05.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_sf06.cpp
SRC += ${PI_FONT_DIR}/prop_cpp/font_sf07.cpp
endif
endif

endif
