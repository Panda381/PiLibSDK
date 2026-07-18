
// ****************************************************************************
//
//                           BarePi keyboard CalcKey
//
// ****************************************************************************

#if USE_CALCKEY		// 1=use BarePi CalcKey, 0=not used (drv_calckey.*)

#ifndef _DRV_CALCKEY_H
#define _DRV_CALCKEY_H

#ifndef CALCKEY_REP_TIME1
#define CALCKEY_REP_TIME1	400	// delta time of first press in [ms]
#endif

#ifndef CALCKEY_REP_TIME2
#define CALCKEY_REP_TIME2	100	// delta time of repeat press in [ms]
#endif

#ifndef CALCKEY_TIME
#define CALCKEY_TIME		30	// interval of CalcKey service in [ms]
#endif

#ifndef CALCKEY_I2C
#define CALCKEY_I2C		0	// CalcKey I2C bus
#endif

#ifndef CALCKEY_ADDR
#define CALCKEY_ADDR		0x35	// CalcKey I2C address
#endif

#ifndef CALCKEY_SPEED
#define CALCKEY_SPEED		100000	// CalcKey I2C speed
#endif

#if (CALCKEY_I2C != 1) && (!USE_I2CBUS || !USE_I2C0BUS)
#error "CalcKey requires I2C0 bus driver"
#endif

#if (CALCKEY_I2C != 0) && (!USE_I2CBUS || !USE_I2C1BUS)
#error "CalcKey requires I2C1 bus driver"
#endif

// ===== key codes

#define CALCKEY_0		0x00 // digit 0
#define CALCKEY_1		0x01 // digit 1
#define CALCKEY_2		0x02 // digit 2
#define CALCKEY_3		0x03 // digit 3
#define CALCKEY_4		0x04 // digit 4
#define CALCKEY_5		0x05 // digit 5
#define CALCKEY_6		0x06 // digit 6
#define CALCKEY_7		0x07 // digit 7
#define CALCKEY_8		0x08 // digit 8
#define CALCKEY_9		0x09 // digit 9
#define CALCKEY_0A		0x0a // digit 0A
#define CALCKEY_0B		0x0b // digit 0B
#define CALCKEY_0C		0x0c // digit 0C
#define CALCKEY_0D		0x0d // digit 0D
#define CALCKEY_0E		0x0e // digit 0E
#define CALCKEY_0F		0x0f // digit 0F
                             
#define CALCKEY_E2		0x10 // label E'
#define CALCKEY_A		0x11 // label A
#define CALCKEY_B		0x12 // label B
#define CALCKEY_C		0x13 // label C
#define CALCKEY_D		0x14 // label D
#define CALCKEY_E		0x15 // label E
#define CALCKEY_A2		0x16 // label A'
#define CALCKEY_B2		0x17 // label B'
#define CALCKEY_C2		0x18 // label C'
#define CALCKEY_D2		0x19 // label D'
#define CALCKEY_A3		0x1a // label A''
#define CALCKEY_B3		0x1b // label B''
#define CALCKEY_C3		0x1c // label C''
#define CALCKEY_D3		0x1d // label D''
#define CALCKEY_E3		0x1e // label E''
#define CALCKEY_F		0x1f // label F
                             
#define CALCKEY_OFF		0x20 // OFF (ON)
#define CALCKEY_2ND		0x21 // 2nd
#define CALCKEY_INV		0x22 // INV
#define CALCKEY_LNX		0x23 // Ln x
#define CALCKEY_CE		0x24 // CE
#define CALCKEY_CLR		0x25 // CLR
#define CALCKEY_SBR_IND		0x26 // SBR Ind
#define CALCKEY_HIR_IND		0x27 // HIR Ind
#define CALCKEY_LOG		0x28 // log
#define CALCKEY_CP		0x29 // CP
//				0x2a //
#define CALCKEY_CODE		0x2b // code
#define CALCKEY_LG2		0x2c // log2
#define CALCKEY_RAND		0x2d // rand
//				0x2e //
//				0x2f // 
                             
#define CALCKEY_TAN		0x30 // tan
#define CALCKEY_LRN		0x31 // LRN
#define CALCKEY_XT		0x32 // x<>t
#define CALCKEY_X2		0x33 // x^2
#define CALCKEY_SQR		0x34 // Vx
#define CALCKEY_1X		0x35 // 1/x
#define CALCKEY_PGM		0x36 // Pgm
#define CALCKEY_PR		0x37 // P->R
#define CALCKEY_SIN		0x38 // sin
#define CALCKEY_COS		0x39 // cos
#define CALCKEY_TEMP		0x3a // Temp
#define CALCKEY_XY		0x3b // x<>y
#define CALCKEY_SINH		0x3c // sinh
#define CALCKEY_COSH		0x3d // cosh
#define CALCKEY_TANH		0x3e // tanh
//				0x3f // 
                             
#define CALCKEY_IND		0x40 // Ind
#define CALCKEY_SST		0x41 // SST
#define CALCKEY_STO		0x42 // STO
#define CALCKEY_RCL		0x43 // RCL
#define CALCKEY_SUM		0x44 // SUM
#define CALCKEY_POW		0x45 // y^x
#define CALCKEY_INS		0x46 // Ins
#define CALCKEY_CMS		0x47 // CMs
#define CALCKEY_EXC		0x48 // Exc
#define CALCKEY_PRD		0x49 // Prd
#define CALCKEY_BAT		0x4a // BAT
#define CALCKEY_FACT		0x4b // n!
#define CALCKEY_LNFACT		0x4c // ln n!
#define CALCKEY_LOGFACT		0x4d // log n!
#define CALCKEY_MOD2		0x4e // mod2 (floor)
//				0x4f // 
                             
#define CALCKEY_ABS		0x50 // |x|
#define CALCKEY_BST		0x51 // BST
#define CALCKEY_EE		0x52 // EE
#define CALCKEY_LPAR		0x53 // (
#define CALCKEY_RPAR		0x54 // )
#define CALCKEY_DIV		0x55 // :
#define CALCKEY_DEL		0x56 // Del
#define CALCKEY_ENG		0x57 // Eng
#define CALCKEY_FIX		0x58 // Fix
#define CALCKEY_INT		0x59 // Int
#define CALCKEY_LCD		0x5a // LCD
#define CALCKEY_LEFT		0x5b // <<
#define CALCKEY_RIGHT		0x5c // >>
#define CALCKEY_ROUND		0x5d // round
#define CALCKEY_MOD		0x5e // mod (trunc)
//				0x5f // 
                             
#define CALCKEY_DEG		0x60 // Deg
#define CALCKEY_GTO		0x61 // GTO
#define CALCKEY_PGM_IND		0x62 // Pgm Ind
#define CALCKEY_EXC_IND		0x63 // Exc Ind
#define CALCKEY_PRD_IND		0x64 // Prd Ind
#define CALCKEY_MUL		0x65 // x
#define CALCKEY_PAU		0x66 // Pause
#define CALCKEY_EQ		0x67 // x=t
#define CALCKEY_NOP		0x68 // Nop
#define CALCKEY_OP		0x69 // Op
#define CALCKEY_REL		0x6a // Rel
#define CALCKEY_INC_IND		0x6b // Inc Ind
#define CALCKEY_REG_IND		0x6c // Reg Ind
#define CALCKEY_IF_IND		0x6d // If Ind
#define CALCKEY_AND		0x6e // AND &
//				0x6f // 
                             
#define CALCKEY_RAD		0x70 // Rad
#define CALCKEY_SBR		0x71 // SBR
#define CALCKEY_STO_IND		0x72 // STO Ind
#define CALCKEY_RCL_IND		0x73 // RCL Ind
#define CALCKEY_SUM_IND		0x74 // SUM Ind
#define CALCKEY_SUB		0x75 // -
#define CALCKEY_LBL		0x76 // Lbl
#define CALCKEY_GE		0x77 // x>=t
#define CALCKEY_STA		0x78 // Stat+
#define CALCKEY_AVR		0x79 // Avrg x (Mean)
#define CALCKEY_IF		0x7a // If
//				0x7b // 
//				0x7c // 
//				0x7d // 
#define CALCKEY_XOR		0x7e // XOR ~
//				0x7f // 
                             
#define CALCKEY_GRD		0x80 // Grad
#define CALCKEY_RST		0x81 // RST
#define CALCKEY_HIR		0x82 // HIR
#define CALCKEY_GTO_IND		0x83 // GTO Ind
#define CALCKEY_OP_IND		0x84 // Op Ind
#define CALCKEY_ADD		0x85 // +
#define CALCKEY_STF		0x86 // St Flg
#define CALCKEY_IFF		0x87 // If Flg
#define CALCKEY_DMS		0x88 // D.MS
#define CALCKEY_PI		0x89 // pi
#define CALCKEY_REG		0x8a // Reg
#define CALCKEY_HEX		0x8b // HEX
#define CALCKEY_BIN		0x8c // BIN
#define CALCKEY_OCR		0x8d // OCT
#define CALCKEY_OR		0x8e // OR |
//				0x8f // 
                             
#define CALCKEY_LST		0x90 // List
#define CALCKEY_RS		0x91 // R/S
#define CALCKEY_RTN		0x92 // RTN
#define CALCKEY_DOT		0x93 // .
#define CALCKEY_NEG		0x94 // +/-
#define CALCKEY_RES		0x95 // =
#define CALCKEY_WRT		0x96 // Write
#define CALCKEY_DSZ		0x97 // Dsz
#define CALCKEY_ADV		0x98 // Adv
#define CALCKEY_PRT		0x99 // Prt
#define CALCKEY_PHI		0x9a // phi
#define CALCKEY_DEC		0x9b // DEC
#define CALCKEY_INC		0x9c // Inc
#define CALCKEY_NOT		0x9d // NOT
#define CALCKEY_PERC		0x9e // %
//				0x9f // 

#define CALC_NOKEY		0xff	// no key
//#define CALC_MAXKEY		0x9f // max. valid key
#define CALCKEY_NUM		0xA0	// max. number of keys

#define CALCKEY_2ND_1	0	// normal state
#define CALCKEY_2ND_2	1	// 2nd state
#define CALCKEY_2ND_3	2	// 3rd state

// keyboard is initialized
extern Bool CalcKeyIsInit;	// keyboard is initialized
extern u32 CalcKeyTime;		// last time of CalcKey service

// key map
extern volatile Bool CalcKeyMap[CALCKEY_NUM];	// key pressed flags
extern volatile u32 CalcKeyPressTime[CALCKEY_NUM]; // time of next press
extern int CalcKey2nd;			// 2nd flag CALCKEY_2ND_*

// keyboard buffer
#define CALCKEYBUF_SIZE	8		// size of keyboard buffer
extern u8 CalcKeyBuf[CALCKEYBUF_SIZE];	// keyboard buffer
extern u8 CalcKeyRetBuf;		// returned key to keyboard buffer (CALC_NOKEY = no key)
extern volatile int CalcKeyWriteOff;	// write offset to keyboard buffer
extern volatile int CalcKeyReadOff;	// read offset from keyboard buffer

// key names for display
extern const char* CalcDispName[0xa0];

// keyboard initialize
// - Must be initialized after initialization of the I2C bus driver.
void CalcKeyInit(void);

// keyboard terminate
void CalcKeyTerm(void);

// check if key CALCKEY_* is pressed (must be valid key code)
INLINE Bool CalcKeyPressed(u8 key) { return CalcKeyMap[key]; }

// scan keyboard (called from SysTick handler)
void CalcKeyScan(void);

// get raw key code from keyboard buffer (returns CALC_NOKEY if no key code)
u8 CalcKeyGetRaw(void);

// get key code from keyboard buffer (returns CALC_NOKEY if no key code)
u8 CalcKeyGet(void);

// flush keyboard buffer
void CalcKeyFlush(void);

// return raw key to keyboard buffer (can hold only 1 key)
void CalcKeyRet(u8 key);

// check no pressed key
Bool CalcKeyNoPressed(void);

// wait for no key pressed
void CalcKeyWaitNoPressed(void);

#endif // _DRV_CALCKEY_H

#endif // USE_CALCKEY
