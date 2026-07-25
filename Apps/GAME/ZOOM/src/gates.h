
// ****************************************************************************
//
//                               Gates
//
// ****************************************************************************

#ifndef _GATES_H
#define _GATES_H

#define GATES		3	// number of gates
#define	GATE_DISTX	100	// distance X of the gate pillars
#define GATEFREE	-32768	// flag - unused gate
#define GATE_DISTZ	80	// distance Z of gates
#define GATE_INITZ	110	// start Z of gate
#define GATE_PASS_DX	40	// X tolerance for passing through the gate
#define GATE_CRASH_DX	100	// X tolerance for crashing the gate

// gate descriptor
typedef struct {
	short	x;	// X middle coordinate (GATEFREE = unused gate)
	short	z;	// Z coordinate, with FRAC fraction
	Bool	def;	// gate defused
	Bool	bomb;	// bomb released
} sGate;

extern sGate	Gates[GATES];	// gates
extern int	GateNext;	// distance counter to generate next gate

// image width: 192 pixels
// image height: 40 lines
extern u8* GateImg;
#define GATE_IMG_W	24
#define GATE_IMG_H	40

extern const u8 GateSnd[];

// reset gates on start of new scene
void ResetGates();

// display gates
void DispGate();

// gate service (dz = delta Z coordinate, with FRAC fraction)
void Gating(int dz);

#endif // _GATES_H
