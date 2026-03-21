
// ****************************************************************************
//
//                                  Font files
//
// ****************************************************************************

#include "../includes.h"	// includes

// Fixed fonts (image of 128 characters)
#include "fixed_c/font_bold_8x16.c"
#if USE_FONTFIX		// 1=include all fixed fonts, 0=include only default font FontBold8x16
#include "fixed_c/font_bold_8x12.c"
#include "fixed_c/font_bold_8x14.c"
#include "fixed_c/font_bold_8x14B.c"
#include "fixed_c/font_bold_8x16B.c"
#include "fixed_c/font_bold_8x8.c"
#include "fixed_c/font_cond_6x6.c"
#include "fixed_c/font_cond_6x6B.c"
#include "fixed_c/font_cond_6x8.c"
#include "fixed_c/font_game_8x8.c"
#include "fixed_c/font_ibm_8x14.c"
#include "fixed_c/font_ibm_8x16.c"
#include "fixed_c/font_ibm_8x8.c"
#include "fixed_c/font_ibmtiny_8x8.c"
#include "fixed_c/font_italic_8x8.c"
#include "fixed_c/font_thin_8x8.c"
#include "fixed_c/font_tiny_5x8.c"
#include "fixed_c/font_zx.c"
// Extra fonts with special format (image of 64 characters)
#include "fixed_c/font_80.c"
#include "fixed_c/font_81.c"
#endif // USE_FONTFIX

// Proportional fonts
#if USE_FONTPROP		// 1=use support of proportional fonts, 0=not used
#include "prop_c/font_arb10.c"
#if USE_FONTPROP_ARIAL		// 1=include Arial fonts, 0=include only default font FontARB10
#include "prop_c/font_ar08.c"
#include "prop_c/font_ar09.c"
#include "prop_c/font_ar10.c"
#include "prop_c/font_ar11.c"
#include "prop_c/font_ar12.c"
#include "prop_c/font_ar14.c"
#include "prop_c/font_arb08.c"
#include "prop_c/font_arb09.c"
#include "prop_c/font_arb11.c"
#include "prop_c/font_arb12.c"
#include "prop_c/font_arb14.c"
#endif // USE_FONTPROP_ARIAL
#if USE_FONTPROP_GARAMOND	// 1=include Garamond fonts, 0=not included
#include "prop_c/font_gmb07.c"
#include "prop_c/font_gmb08.c"
#include "prop_c/font_gmb09.c"
#include "prop_c/font_gmb10.c"
#include "prop_c/font_gmb11.c"
#include "prop_c/font_gmb12.c"
#include "prop_c/font_gmb14.c"
#include "prop_c/font_gmb16.c"
#include "prop_c/font_gmb18.c"
#include "prop_c/font_gmb20.c"
#endif // USE_FONTPROP_GARAMOND
#if USE_FONTPROP_LUCIDA		// 1=include Lucida Grande fonts, 0=not included
#include "prop_c/font_lg08.c"
#include "prop_c/font_lg09.c"
#include "prop_c/font_lgb07.c"
#include "prop_c/font_lgb08.c"
#include "prop_c/font_lgb09.c"
#include "prop_c/font_lgb10.c"
#include "prop_c/font_lgb11.c"
#include "prop_c/font_lgb12.c"
#include "prop_c/font_lgb14.c"
#include "prop_c/font_lgb16.c"
#include "prop_c/font_lgb18.c"
#include "prop_c/font_lgb20.c"
#endif // USE_FONTPROP_LUCIDA
#if USE_FONTPROP_SABON		// 1=include Sabon fonts, 0=not included
#include "prop_c/font_sb08.c"
#include "prop_c/font_sb08x.c"
#include "prop_c/font_sb09.c"
#include "prop_c/font_sb09x.c"
#include "prop_c/font_sb10.c"
#include "prop_c/font_sb10x.c"
#include "prop_c/font_sb11.c"
#include "prop_c/font_sb11x.c"
#include "prop_c/font_sb12.c"
#include "prop_c/font_sb12x.c"
#include "prop_c/font_sb14.c"
#include "prop_c/font_sb14x.c"
#include "prop_c/font_sb16.c"
#include "prop_c/font_sb16x.c"
#include "prop_c/font_sbr10.c"
#include "prop_c/font_sbr11.c"
#include "prop_c/font_sbr11x.c"
#endif // USE_FONTPROP_SABON
#if USE_FONTPROP_TAHOMA		// 1=include Tahoma fonts, 0=not included
#include "prop_c/font_tm07.c"
#include "prop_c/font_tm08.c"
#include "prop_c/font_tm09.c"
#include "prop_c/font_tm10.c"
#include "prop_c/font_tmb07.c"
#include "prop_c/font_tmb08.c"
#include "prop_c/font_tmb09.c"
#include "prop_c/font_tmb10.c"
#endif // USE_FONTPROP_TAHOMA
#if USE_FONTPROP_MIXED		// 1=include mixed fonts, 0=not included
#include "prop_c/font_digital.c"
#include "prop_c/font_klvkb36.c"
#include "prop_c/font_mss08.c"
#include "prop_c/font_sf05.c"
#include "prop_c/font_sf06.c"
#include "prop_c/font_sf07.c"
#endif // USE_FONTPROP_MIXED
#endif // USE_FONTPROP
