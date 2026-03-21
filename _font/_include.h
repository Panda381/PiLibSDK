
#ifndef _FONT_INCLUDE_H
#define _FONT_INCLUDE_H

// Fixed fonts (image of 128 characters)
extern const ALIGNED u8 FontBold8x16[2048];
#if USE_FONTFIX		// 1=include all fixed fonts, 0=include only default font FontBold8x16
extern const ALIGNED u8 FontBold8x8[1024];
extern const ALIGNED u8 FontBold8x12[1536];
extern const ALIGNED u8 FontBold8x14[1792];
extern const ALIGNED u8 FontBold8x14B[1792];
extern const ALIGNED u8 FontBold8x16B[2048];
extern const ALIGNED u8 FontCond6x6[768];
extern const ALIGNED u8 FontCond6x6B[768];
extern const ALIGNED u8 FontCond6x8[1024];
extern const ALIGNED u8 FontGame8x8[1024];
extern const ALIGNED u8 FontIbm8x8[1024];
extern const ALIGNED u8 FontIbm8x14[1792];
extern const ALIGNED u8 FontIbm8x16[2048];
extern const ALIGNED u8 FontIbmTiny8x8[1024];
extern const ALIGNED u8 FontItalic8x8[1024];
extern const ALIGNED u8 FontThin8x8[1024];
extern const ALIGNED u8 FontTiny5x8[1024];
extern const ALIGNED u8 FontZx[1024];
// Extra fonts with special format (image of 64 characters) ... cannot be used in DrawText and DrawChar functions
extern const ALIGNED u8 Font80[512];
extern const ALIGNED u8 Font81[512];
#endif // USE_FONTFIX

// Proportional fonts
#if USE_FONTPROP		// 1=use support of proportional fonts, 0=not used
extern const u8 FontARB10[9484];	// Arial Bold 10pt
#if USE_FONTPROP_ARIAL		// 1=include Arial fonts, 0=include only default font FontARB10
extern const u8 FontAR08[6626];		// Arial Regular 8pt
extern const u8 FontAR09[7491];		// Arial Regular 9pt
extern const u8 FontAR10[8604];		// Arial Regular 10pt
extern const u8 FontAR11[12186];	// Arial Regular 11pt
extern const u8 FontAR12[14152];	// Arial Regular 12pt
extern const u8 FontAR14[19646];	// Arial Regular 14pt
extern const u8 FontARB08[7452];	// Arial Bold 8pt
extern const u8 FontARB09[8646];	// Arial Bold 9pt
extern const u8 FontARB11[12186];	// Arial Bold 11pt
extern const u8 FontARB12[14152];	// Arial Bold 12pt
extern const u8 FontARB14[19646];	// Arial Bold 14pt
#endif // USE_FONTPROP_ARIAL
#if USE_FONTPROP_GARAMOND	// 1=include Garamond fonts, 0=not included
extern const u8 FontGMB07[7078];	// Garamond Bold 7pt
extern const u8 FontGMB08[8208];	// Garamond Bold 8pt
extern const u8 FontGMB09[10834];	// Garamond Bold 9pt
extern const u8 FontGMB10[12042];	// Garamond Bold 10pt
extern const u8 FontGMB11[14418];	// Garamond Bold 11pt
extern const u8 FontGMB12[17622];	// Garamond Bold 12pt
extern const u8 FontGMB14[23596];	// Garamond Bold 14pt
extern const u8 FontGMB16[26864];	// Garamond Bold 16pt
extern const u8 FontGMB18[34776];	// Garamond Bold 18pt
extern const u8 FontGMB20[44086];	// Garamond Bold 20pt
#endif // USE_FONTPROP_GARAMOND
#if USE_FONTPROP_LUCIDA		// 1=include Lucida Grande fonts, 0=not included
extern const u8 FontLG08[9379];		// Lucida Grande 8pt
extern const u8 FontLG09[13668];	// Lucida Grande 9pt
extern const u8 FontLGB07[8760];	// Lucida Grande Bold 7pt
extern const u8 FontLGB08[9951];	// Lucida Grande Bold 8pt
extern const u8 FontLGB09[14242];	// Lucida Grande Bold 9pt
extern const u8 FontLGB10[17372];	// Lucida Grande Bold 10pt
extern const u8 FontLGB11[22666];	// Lucida Grande Bold 11pt
extern const u8 FontLGB12[26521];	// Lucida Grande Bold 12pt
extern const u8 FontLGB14[37378];	// Lucida Grande Bold 14pt
extern const u8 FontLGB16[44796];	// Lucida Grande Bold 16pt
extern const u8 FontLGB18[58972];	// Lucida Grande Bold 18pt
extern const u8 FontLGB20[69340];	// Lucida Grande Bold 20pt
#endif // USE_FONTPROP_LUCIDA
#if USE_FONTPROP_SABON		// 1=include Sabon fonts, 0=not included
extern const u8 FontSB08[16342];	// Sabon 8pt
extern const u8 FontSB08X[16342];	// Sabon 8pt shadow
extern const u8 FontSB09[20276];	// Sabon 9pt
extern const u8 FontSB09X[20276];	// Sabon 9pt shadow
extern const u8 FontSB10[21963];	// Sabon 10pt
extern const u8 FontSB10X[21963];	// Sabon 10pt shadow
extern const u8 FontSB11[26110];	// Sabon 11pt
extern const u8 FontSB11X[26110];	// Sabon 11pt shadow
extern const u8 FontSB12[28308];	// Sabon 12pt
extern const u8 FontSB12X[28308];	// Sabon 12pt shadow
extern const u8 FontSB14[35712];	// Sabon 14pt
extern const u8 FontSB14X[35712];	// Sabon 14pt shadow
extern const u8 FontSB16[43734];	// Sabon 16pt
extern const u8 FontSB16X[43734];	// Sabon 16pt shadow
extern const u8 FontSBR10[11395];	// Sabon 10pt Regular
extern const u8 FontSBR11[26110];	// Sabon 11pt raised
extern const u8 FontSBR11X[26110];	// Sabon 11pt raised shadow
#endif // USE_FONTPROP_SABON
#if USE_FONTPROP_TAHOMA		// 1=include Tahoma fonts, 0=not included
extern const u8 FontTM07[4807];		// Tahoma 7pt
extern const u8 FontTM08[6285];		// Tahoma 8pt
extern const u8 FontTM09[7550];		// Tahoma 9pt
extern const u8 FontTM10[8988];		// Tahoma 10pt
extern const u8 FontTMB07[5852];	// Tahoma Bold 7pt
extern const u8 FontTMB08[7546];	// Tahoma Bold 8pt
extern const u8 FontTMB09[8810];	// Tahoma Bold 9pt
extern const u8 FontTMB10[10428];	// Tahoma Bold 10pt
#endif // USE_FONTPROP_TAHOMA
#if USE_FONTPROP_MIXED		// 1=include mixed fonts, 0=not included
extern const u8 FontDigital[2959];	// digits
extern const u8 FontKLVKB36[5334];	// Klavika digits Bold 36pt
extern const u8 FontMSS08[5986];	// MS Sans Serif 8pt
extern const u8 FontSF05[3292];		// Small font 5pt
extern const u8 FontSF06[4496];		// Small font 6pt
extern const u8 FontSF07[5412];		// Small font 7pt
#endif // USE_FONTPROP_MIXED
#endif // USE_FONTPROP

#endif // _FONT_INCLUDE_H
