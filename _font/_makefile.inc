
DEFINE += -D USE_FONTFIX=$(USE_FONTFIX)
DEFINE += -D USE_FONTPROP=$(USE_FONTPROP)
DEFINE += -D USE_FONTPROP_ARIAL=$(USE_FONTPROP_ARIAL)
DEFINE += -D USE_FONTPROP_GARAMOND=$(USE_FONTPROP_GARAMOND)
DEFINE += -D USE_FONTPROP_LUCIDA=$(USE_FONTPROP_LUCIDA)
DEFINE += -D USE_FONTPROP_SABON=$(USE_FONTPROP_SABON)
DEFINE += -D USE_FONTPROP_TAHOMA=$(USE_FONTPROP_TAHOMA)
DEFINE += -D USE_FONTPROP_MIXED=$(USE_FONTPROP_MIXED)

ifeq (${OS},Windows_NT)
CSRC += ${PI_FONT_DIR}/font.c
else

CSRC += ${PI_FONT_DIR}/fixed_c/font_bold_8x16.c
ifeq (${USE_FONTFIX},1)
CSRC += ${PI_FONT_DIR}/fixed_c/font_bold_8x12.c
CSRC += ${PI_FONT_DIR}/fixed_c/font_bold_8x14.c
CSRC += ${PI_FONT_DIR}/fixed_c/font_bold_8x14B.c
CSRC += ${PI_FONT_DIR}/fixed_c/font_bold_8x16B.c
CSRC += ${PI_FONT_DIR}/fixed_c/font_bold_8x8.c
CSRC += ${PI_FONT_DIR}/fixed_c/font_cond_6x6.c
CSRC += ${PI_FONT_DIR}/fixed_c/font_cond_6x6B.c
CSRC += ${PI_FONT_DIR}/fixed_c/font_cond_6x8.c
CSRC += ${PI_FONT_DIR}/fixed_c/font_game_8x8.c
CSRC += ${PI_FONT_DIR}/fixed_c/font_ibm_8x14.c
CSRC += ${PI_FONT_DIR}/fixed_c/font_ibm_8x16.c
CSRC += ${PI_FONT_DIR}/fixed_c/font_ibm_8x8.c
CSRC += ${PI_FONT_DIR}/fixed_c/font_ibmtiny_8x8.c
CSRC += ${PI_FONT_DIR}/fixed_c/font_italic_8x8.c
CSRC += ${PI_FONT_DIR}/fixed_c/font_thin_8x8.c
CSRC += ${PI_FONT_DIR}/fixed_c/font_tiny_5x8.c
CSRC += ${PI_FONT_DIR}/fixed_c/font_zx.c
CSRC += ${PI_FONT_DIR}/fixed_c/font_80.c
CSRC += ${PI_FONT_DIR}/fixed_c/font_81.c
endif
ifeq (${USE_FONTPROP},1)
CSRC += ${PI_FONT_DIR}/prop_c/font_arb10.c
ifeq (${USE_FONTPROP_ARIAL},1)
CSRC += ${PI_FONT_DIR}/prop_c/font_ar08.c
CSRC += ${PI_FONT_DIR}/prop_c/font_ar09.c
CSRC += ${PI_FONT_DIR}/prop_c/font_ar10.c
CSRC += ${PI_FONT_DIR}/prop_c/font_ar11.c
CSRC += ${PI_FONT_DIR}/prop_c/font_ar12.c
CSRC += ${PI_FONT_DIR}/prop_c/font_ar14.c
CSRC += ${PI_FONT_DIR}/prop_c/font_arb08.c
CSRC += ${PI_FONT_DIR}/prop_c/font_arb09.c
CSRC += ${PI_FONT_DIR}/prop_c/font_arb11.c
CSRC += ${PI_FONT_DIR}/prop_c/font_arb12.c
CSRC += ${PI_FONT_DIR}/prop_c/font_arb14.c
endif
ifeq (${USE_FONTPROP_GARAMOND},1)
CSRC += ${PI_FONT_DIR}/prop_c/font_gmb07.c
CSRC += ${PI_FONT_DIR}/prop_c/font_gmb08.c
CSRC += ${PI_FONT_DIR}/prop_c/font_gmb09.c
CSRC += ${PI_FONT_DIR}/prop_c/font_gmb10.c
CSRC += ${PI_FONT_DIR}/prop_c/font_gmb11.c
CSRC += ${PI_FONT_DIR}/prop_c/font_gmb12.c
CSRC += ${PI_FONT_DIR}/prop_c/font_gmb14.c
CSRC += ${PI_FONT_DIR}/prop_c/font_gmb16.c
CSRC += ${PI_FONT_DIR}/prop_c/font_gmb18.c
CSRC += ${PI_FONT_DIR}/prop_c/font_gmb20.c
endif
ifeq (${USE_FONTPROP_LUCIDA},1)
CSRC += ${PI_FONT_DIR}/prop_c/font_lg08.c
CSRC += ${PI_FONT_DIR}/prop_c/font_lg09.c
CSRC += ${PI_FONT_DIR}/prop_c/font_lgb07.c
CSRC += ${PI_FONT_DIR}/prop_c/font_lgb08.c
CSRC += ${PI_FONT_DIR}/prop_c/font_lgb09.c
CSRC += ${PI_FONT_DIR}/prop_c/font_lgb10.c
CSRC += ${PI_FONT_DIR}/prop_c/font_lgb11.c
CSRC += ${PI_FONT_DIR}/prop_c/font_lgb12.c
CSRC += ${PI_FONT_DIR}/prop_c/font_lgb14.c
CSRC += ${PI_FONT_DIR}/prop_c/font_lgb16.c
CSRC += ${PI_FONT_DIR}/prop_c/font_lgb18.c
CSRC += ${PI_FONT_DIR}/prop_c/font_lgb20.c
endif
ifeq (${USE_FONTPROP_SABON},1)
CSRC += ${PI_FONT_DIR}/prop_c/font_sb08.c
CSRC += ${PI_FONT_DIR}/prop_c/font_sb08x.c
CSRC += ${PI_FONT_DIR}/prop_c/font_sb09.c
CSRC += ${PI_FONT_DIR}/prop_c/font_sb09x.c
CSRC += ${PI_FONT_DIR}/prop_c/font_sb10.c
CSRC += ${PI_FONT_DIR}/prop_c/font_sb10x.c
CSRC += ${PI_FONT_DIR}/prop_c/font_sb11.c
CSRC += ${PI_FONT_DIR}/prop_c/font_sb11x.c
CSRC += ${PI_FONT_DIR}/prop_c/font_sb12.c
CSRC += ${PI_FONT_DIR}/prop_c/font_sb12x.c
CSRC += ${PI_FONT_DIR}/prop_c/font_sb14.c
CSRC += ${PI_FONT_DIR}/prop_c/font_sb14x.c
CSRC += ${PI_FONT_DIR}/prop_c/font_sb16.c
CSRC += ${PI_FONT_DIR}/prop_c/font_sb16x.c
CSRC += ${PI_FONT_DIR}/prop_c/font_sbr10.c
CSRC += ${PI_FONT_DIR}/prop_c/font_sbr11.c
CSRC += ${PI_FONT_DIR}/prop_c/font_sbr11x.c
endif
ifeq (${USE_FONTPROP_TAHOMA},1)
CSRC += ${PI_FONT_DIR}/prop_c/font_tm07.c
CSRC += ${PI_FONT_DIR}/prop_c/font_tm08.c
CSRC += ${PI_FONT_DIR}/prop_c/font_tm09.c
CSRC += ${PI_FONT_DIR}/prop_c/font_tm10.c
CSRC += ${PI_FONT_DIR}/prop_c/font_tmb07.c
CSRC += ${PI_FONT_DIR}/prop_c/font_tmb08.c
CSRC += ${PI_FONT_DIR}/prop_c/font_tmb09.c
CSRC += ${PI_FONT_DIR}/prop_c/font_tmb10.c
endif
ifeq (${USE_FONTPROP_MIXED},1)
CSRC += ${PI_FONT_DIR}/prop_c/font_digital.c
CSRC += ${PI_FONT_DIR}/prop_c/font_klvkb36.c
CSRC += ${PI_FONT_DIR}/prop_c/font_mss08.c
CSRC += ${PI_FONT_DIR}/prop_c/font_sf05.c
CSRC += ${PI_FONT_DIR}/prop_c/font_sf06.c
CSRC += ${PI_FONT_DIR}/prop_c/font_sf07.c
endif
endif

endif
