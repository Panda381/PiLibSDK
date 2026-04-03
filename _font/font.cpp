
// ****************************************************************************
//
//                                  Font files
//
// ****************************************************************************

#include "../includes.h"	// includes

// Fixed fonts (image of 128 characters)
#include "fixed_cpp/font_bold_8x16.cpp"
#if USE_FONTFIX		// 1=include all fixed fonts, 0=include only default font FontBold8x16
#include "fixed_cpp/font_bold_8x12.cpp"
#include "fixed_cpp/font_bold_8x14.cpp"
#include "fixed_cpp/font_bold_8x14B.cpp"
#include "fixed_cpp/font_bold_8x16B.cpp"
#include "fixed_cpp/font_bold_8x8.cpp"
#include "fixed_cpp/font_cond_6x6.cpp"
#include "fixed_cpp/font_cond_6x6B.cpp"
#include "fixed_cpp/font_cond_6x8.cpp"
#include "fixed_cpp/font_game_8x8.cpp"
#include "fixed_cpp/font_ibm_8x14.cpp"
#include "fixed_cpp/font_ibm_8x16.cpp"
#include "fixed_cpp/font_ibm_8x8.cpp"
#include "fixed_cpp/font_ibmtiny_8x8.cpp"
#include "fixed_cpp/font_italic_8x8.cpp"
#include "fixed_cpp/font_thin_8x8.cpp"
#include "fixed_cpp/font_tiny_5x8.cpp"
#include "fixed_cpp/font_zx.cpp"
// Extra fonts with special format (image of 64 characters)
#include "fixed_cpp/font_80.cpp"
#include "fixed_cpp/font_81.cpp"
#endif // USE_FONTFIX

// Proportional fonts
#if USE_FONTPROP		// 1=use support of proportional fonts, 0=not used
#include "prop_cpp/font_arb10.cpp"
#if USE_FONTPROP_ARIAL		// 1=include Arial fonts, 0=include only default font FontARB10
#include "prop_cpp/font_ar08.cpp"
#include "prop_cpp/font_ar09.cpp"
#include "prop_cpp/font_ar10.cpp"
#include "prop_cpp/font_ar11.cpp"
#include "prop_cpp/font_ar12.cpp"
#include "prop_cpp/font_ar14.cpp"
#include "prop_cpp/font_arb08.cpp"
#include "prop_cpp/font_arb09.cpp"
#include "prop_cpp/font_arb11.cpp"
#include "prop_cpp/font_arb12.cpp"
#include "prop_cpp/font_arb14.cpp"
#endif // USE_FONTPROP_ARIAL
#if USE_FONTPROP_GARAMOND	// 1=include Garamond fonts, 0=not included
#include "prop_cpp/font_gmb07.cpp"
#include "prop_cpp/font_gmb08.cpp"
#include "prop_cpp/font_gmb09.cpp"
#include "prop_cpp/font_gmb10.cpp"
#include "prop_cpp/font_gmb11.cpp"
#include "prop_cpp/font_gmb12.cpp"
#include "prop_cpp/font_gmb14.cpp"
#include "prop_cpp/font_gmb16.cpp"
#include "prop_cpp/font_gmb18.cpp"
#include "prop_cpp/font_gmb20.cpp"
#endif // USE_FONTPROP_GARAMOND
#if USE_FONTPROP_LUCIDA		// 1=include Lucida Grande fonts, 0=not included
#include "prop_cpp/font_lg08.cpp"
#include "prop_cpp/font_lg09.cpp"
#include "prop_cpp/font_lgb07.cpp"
#include "prop_cpp/font_lgb08.cpp"
#include "prop_cpp/font_lgb09.cpp"
#include "prop_cpp/font_lgb10.cpp"
#include "prop_cpp/font_lgb11.cpp"
#include "prop_cpp/font_lgb12.cpp"
#include "prop_cpp/font_lgb14.cpp"
#include "prop_cpp/font_lgb16.cpp"
#include "prop_cpp/font_lgb18.cpp"
#include "prop_cpp/font_lgb20.cpp"
#endif // USE_FONTPROP_LUCIDA
#if USE_FONTPROP_SABON		// 1=include Sabon fonts, 0=not included
#include "prop_cpp/font_sb08.cpp"
#include "prop_cpp/font_sb08x.cpp"
#include "prop_cpp/font_sb09.cpp"
#include "prop_cpp/font_sb09x.cpp"
#include "prop_cpp/font_sb10.cpp"
#include "prop_cpp/font_sb10x.cpp"
#include "prop_cpp/font_sb11.cpp"
#include "prop_cpp/font_sb11x.cpp"
#include "prop_cpp/font_sb12.cpp"
#include "prop_cpp/font_sb12x.cpp"
#include "prop_cpp/font_sb14.cpp"
#include "prop_cpp/font_sb14x.cpp"
#include "prop_cpp/font_sb16.cpp"
#include "prop_cpp/font_sb16x.cpp"
#include "prop_cpp/font_sbr10.cpp"
#include "prop_cpp/font_sbr11.cpp"
#include "prop_cpp/font_sbr11x.cpp"
#endif // USE_FONTPROP_SABON
#if USE_FONTPROP_TAHOMA		// 1=include Tahoma fonts, 0=not included
#include "prop_cpp/font_tm07.cpp"
#include "prop_cpp/font_tm08.cpp"
#include "prop_cpp/font_tm09.cpp"
#include "prop_cpp/font_tm10.cpp"
#include "prop_cpp/font_tmb07.cpp"
#include "prop_cpp/font_tmb08.cpp"
#include "prop_cpp/font_tmb09.cpp"
#include "prop_cpp/font_tmb10.cpp"
#endif // USE_FONTPROP_TAHOMA
#if USE_FONTPROP_MIXED		// 1=include mixed fonts, 0=not included
#include "prop_cpp/font_digital.cpp"
#include "prop_cpp/font_klvkb36.cpp"
#include "prop_cpp/font_mss08.cpp"
#include "prop_cpp/font_sf05.cpp"
#include "prop_cpp/font_sf06.cpp"
#include "prop_cpp/font_sf07.cpp"
#endif // USE_FONTPROP_MIXED
#endif // USE_FONTPROP
