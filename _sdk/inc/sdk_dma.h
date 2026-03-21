
// ****************************************************************************
//
//                            DMA controller
//
// ****************************************************************************
// As DMA address, use BUS addresses (use DMA_BusAddr()). Also pointer to control block.
// ARM local periphery (base address 0x40000000) cannot be accessed by the DMA.
// 15 DMA channels:
//	channel 0..6: normal DMA channels (channels 0..3 are default DMA channels for cores 0..3)
//	channel 7..14: DMA LITE channels (they do not support 2D transfer, cannot ignore read/write, can transfer max. 64 KB)
//	.... channel 15: exclusively used by the VPU, cannot be used
// 	.... DMA channels 0 to 3 are reserved for core fast transfers (see DMA_CH)
//	.... DMA channel 7 is reserved for PWM audio output (see PWMSND_DMACHAN and PWMSND_DMAIRQ)

// If a ring buffer is used for the DMA buffer, it is possible to use the DMABUF
// attribute for a static buffer, which ensures that the buffer is created in
// coherent memory that is not cached. This can be useful when it is necessary
// to immediately access the transferred DMA data so that it is not necessary to
// invalidate the cache memory. However, it is important to remember that uncached
// memory does not allow unaligned access. This applies in particular to the memcpy()
// function, which internally uses unaligned access - its use on unaligned memory can
// therefore cause the system to crash.

// DMA transfer is roughly three times slower than memcpy(). However, one of its
// advantages is that it works well even on uncached, unaligned memory, where
// software access can be slow or crash due to alignment errors. The speed of writing
// to uncached video memory using DMA can be up to 11 times faster than memcpy.

#ifndef _SDK_DMA_H
#define _SDK_DMA_H

#ifdef __cplusplus
extern "C" {
#endif

// DREQ - DMA data request number
#if RASPPI < 4

#define DREQ_NONE		0	// Permanent request (memory)
#define DREQ_DSI		1
#define DREQ_PCM_TX		2
#define DREQ_PCM_RX		3
#define DREQ_SMI		4
#define DREQ_PWM		5
#define DREQ_SPI_TX		6
#define DREQ_SPI_RX		7
#define DREQ_BSC_TX		8	// I2C, SPI slave TX
#define DREQ_BSC_RX		9	// I2C, SPI slave RX
// ... 10 unused
#define DREQ_EMMC		11
#define DREQ_UART_TX		12
#define DREQ_SDHOST		13
#define DREQ_UART_RX		14
#define DREQ_DSI1		15
#define DREQ_SLIMBUS_MCTX	16
#define DREQ_HDMI		17
#define DREQ_SLIMBUS_MCRX	18
#define DREQ_SLIMBUS_DC0	19
#define DREQ_SLIMBUS_DC1	20
#define DREQ_SLIMBUS_DC2	21
#define DREQ_SLIMBUS_DC3	22
#define DREQ_SLIMBUS_DC4	23
#define DREQ_SCALER_FIFO0	24	// Scaler FIF0 0 & SMI, SMI can be disabled with SMI_DISABLE
#define DREQ_SCALER_FIFO1	25	// Scaler FIF0 1 & SMI, SMI can be disabled with SMI_DISABLE
#define DREQ_SCALER_FIFO2	26	// Scaler FIF0 2 & SMI, SMI can be disabled with SMI_DISABLE
#define DREQ_SLIMBUS_DC5	27
#define DREQ_SLIMBUS_DC6	28
#define DREQ_SLIMBUS_DC7	29
#define DREQ_SLIMBUS_DC8	30
#define DREQ_SLIMBUS_DC9	31

#else // RASPPI < 4

#define DREQ_NONE		0	// Permanent request (memory)
#define DREQ_DSI0_PWM1		1	// DSI0 or PWM1 - selected in DMA_CNTRL_MUX
#define DREQ_PCM_TX		2
#define DREQ_PCM_RX		3
#define DREQ_SMI		4
#define DREQ_PWM		5	// PWM0
#define DREQ_SPI0_TX		6
#define DREQ_SPI0_RX		7
#define DREQ_BSC_TX		8	// I2C, SPI slave TX
#define DREQ_BSC_RX		9	// I2C, SPI slave RX
#define DREQ_HDMI0		10
#define DREQ_EMMC		11
#define DREQ_UART0_TX		12
#define DREQ_SDHOST		13
#define DREQ_UART0_RX		14
#define DREQ_DSI1		15
#define DREQ_SPI1_TX		16
#define DREQ_HDMI1		17
#define DREQ_SPI1_RX		18
#define DREQ_UART3_SPI4_TX	19	// UART3 or SPI4 - selected in DMA_CNTRL_MUX
#define DREQ_UART3_SPI4_RX	20	// UART3 or SPI4 - selected in DMA_CNTRL_MUX
#define DREQ_UART5_SPI5_TX	21	// UART5 or SPI5 - selected in DMA_CNTRL_MUX
#define DREQ_UART5_SPI5_RX	22	// UART5 or SPI5 - selected in DMA_CNTRL_MUX
#define DREQ_SPI6_TX		23
#define DREQ_SCALER_FIFO0	24	// Scaler FIF0 0 & SMI, SMI can be disabled with SMI_DISABLE
#define DREQ_SCALER_FIFO1	25	// Scaler FIF0 1 & SMI, SMI can be disabled with SMI_DISABLE
#define DREQ_SCALER_FIFO2	26	// Scaler FIF0 2 & SMI, SMI can be disabled with SMI_DISABLE
#define DREQ_SPI6_RX		27
#define DREQ_UART2_TX		28
#define DREQ_UART2_RX		29
#define DREQ_UART4_TX		30
#define DREQ_UART4_RX		31

#endif // RASPPI < 4

// DMA control block (coresponds to registers TI..NEXT of the DMA channel)
// For description of the registers - see DMA_t structure.
// This structure will be loaded into registers of DMA channel after activating transfer (setting bit 0 of CS to 1)
// This structure must be 32-byte aligned (use __attribute__((aligned(32))))
typedef struct {
	io32	TI;		// 0x00: transfer information
	io32	SRC;		// 0x04: source address
	io32	DST;		// 0x08: destination address
	io32	LEN;		// 0x0C: transfer length
	io32	STRIDE;		// 0x10: 2D mode stride
	io32	NEXT;		// 0x14: address of next DMA control block (NULL = no next control block)
	io32	res[2];		// 0x18 (8 = 0x08): ... reserved (set to 0)
} DMA_CB_t __attribute__((aligned(32)));
STATIC_ASSERT(sizeof(DMA_CB_t) == 0x20, "Incorrect DMA_CB_t!");

#define DMA_BURST_IO		1	// default burst for peripherals transfers
#define DMA_BURST_MEM		4	// default burst for memory transfers
#define DMA_XLENGTH_MAX		65535	// max. length of line in bytes
#define DMA_YLENGTH_MAX		65535	// max. length of line in bytes
#define DMA_PANIC_PRIORITY	15	// default panic priority (0..15)
#define DMA_NORM_PRIORITY	1	// default normal priority (0..15)

#define DMA_CH	  CpuID()		// default DMA channel, used by current CPU core

// DMA channel
typedef struct {
	io32	CS;		// 0x00: control and status
				//	bit 0: "ACTIVE" write 1 to activate DMA (load registers from DMA_CB_t control block)
				//		- set 0 to pause DMA, set 1 to resume transfer
				//		- read: bit will be cleared at end of DMA transfer
				//	bit 1: "END" read 1=transfer of current CB complete (write 1 to clear)
				//	bit 2: "INT" read 1=transfer of CB ends and INTEN is set to 1 (write 1 to clear)
				//	bit 3: (read only) "DREQ" state of DREQ signal: 1=requesting data, 0=no data request
				//	bit 4: (read only) "PAUSED" 1=DMA channel is paused, 0=DMA channel is running
				//		Indicates if DMA is paused, if: 1) active bit has been cleared, 2) DMA is executing wait cycles,
				//			3) debug pause signal has been set, 4) number of outstanding writes has exceeded max count.
				//	bit 5: (read only) "DREQ_STOPS_DMA" 1=DMA channel is paused, 0=DMA channel is runnig
				//		Indicates if DMA is paused and not transfering data due to DREQ being inactive.
				//	bit 6: (read only) "WAITING_FOR_OUTSTANDING_WRITES" 1=DMA channel is waiting for last write to be received
				//	bit 8: (read only) 1=ERROR flag is set, 0=DMA channel is OK (clear by writing to debug register)
				//	bit 16..19: "PRIORITY" AXI normal priority level, 0 (lowest) .. 15 (highest), used when the panic bit of the selected peripheral channel is zero
				//	bit 20..23: "PANIC_PRIORITY" AXI panic priority level: 0 (lowest) .. 15 (highest), used when the panic bit of the selected peripheral channel is 1
				//	bit 28: "WAIT_FOR_OUTSTANDING_WRITES" 1=wait for outstanding AXI writes (DMA will pause if it has more than 13 outstanding writes)
				//	bit 29: "DISDEBUG" disable debug pause signal: 1=DMA will not stop when debug pause signal is asserted
				//	bit 30: (write only) "ABORT" write 1 to abort current DMA CB (DMA will load next CB and continue)
				//	bit 31: (write only) "RESET" write 1 to reset DMA channel

	io32	CB;		// 0x04: current control block address (pointer to DMA_CB_t; must be 32-byte aligned), NULL=no control block
				//	Set address of control block DMA_CB_t and then activate transfer by setting bit "ACTIVE" (bit 0) of CS register
				//	After activating transfer, DMA will load registers TI..NEXT from control block DMA_CB_t at this address.
				//	At end of transfer, this register will be updated from NEXT - stop if NEXT=NULL.

	io32	TI;		// 0x08: transfer information (read-only, loaded from DMA_CB_t control block)
				//  CHANNEL 0..6:
				//	bit 0: "INTEN" 1=generate interrupt when transfer completes
				//	bit 1: "TDMODE" 2D mode 1=2D mode, after transfer XLEN line stride address; 0=linear transfer
				//	bit 3: "WAIT_RESP" 1=wait for AXI write response for each write, 0=do not wait
				//	bit 4: "DEST_INC" 1=destination address incremented (4 or 32, by "DEST_WIDTH"), 0=destination address does not change
				//	bit 5: "DEST_WIDTH" destination transfer width 1=use 128-bit destination write width (16 bytes), 0=use 32-bit destination write width (4 bytes)
				//	bit 6: "DEST_DREQ" destination writes 1=DREQ selected by "PERMAP" will gate destination writes, 0=DREQ has no effect
				//	bit 7: "DEST_IGNORE" ignore writes 1=do not perform destination writes, 0=write data to destination
				//	bit 8: "SRC_INC" 1=source address incremented (4 or 32, by "SRC_WIDTH"), 0=source address does not change
				//	bit 9: "SRC_WIDTH" source transfer width 1=use 128-bit source read width (16 bytes), 0=use 32-bit source read width (4 bytes)
				//	bit 10: "SRC_DREQ" source reads 1=DREQ selected by "PERMAP" will gate source reads, 0=DREQ has no effect
				//	bit 11: "SRC_IGNORE" ignore reads 1=do not perform source reads (use 0 instead, used as fast cache fill), 0=read data from source
				//	bit 12..15: "BURST_LENGTH" burst transfer length 1..15 (recommended 2-8, or 4 as default), in number of 32-bit words, 0=single transfer
				//			Burst is only an optimization recommendation for accelerating block-by-block transfer.
				//	bit 16..20: "PERMAP" indicates peripheral mapping number 1..31, whose ready signal shall be used
				//			to control rate of transfers and whose panic signals will be output on DMA AXI bus.
				//			0 = continuous un-paced transfer
				//	bit 21..25: "WAITS" number of dummy wait cycles after each DMA read or write operation 0..31
				//	bit 26: "NO_WIDE_BURSTS" 1=don't do wide writes as 2 beat AXI bursts (inefficient access mode, default use bursts)
				//  CHANNEL 7..14:
				//	bit 0: "INTEN" 1=generate interrupt when transfer completes
				//	bit 3: "WAIT_RESP" 1=wait for AXI write response for each write, 0=do not wait
				//	bit 4: "DEST_INC" 1=destination address incremented (4 or 32, by "DEST_WIDTH"), 0=destination address does not change
				//	bit 5: "DEST_WIDTH" destination transfer width 1=use 128-bit destination write width (16 bytes), 0=use 32-bit destination write width (4 bytes)
				//	bit 6: "DEST_DREQ" destination writes 1=DREQ selected by "PERMAP" will gate destination writes, 0=DREQ has no effect
				//	bit 8: "SRC_INC" 1=source address incremented (4 or 32, by "SRC_WIDTH"), 0=source address does not change
				//	bit 9: "SRC_WIDTH" source transfer width 1=use 128-bit source read width (16 bytes), 0=use 32-bit source read width (4 bytes)
				//	bit 10: "SRC_DREQ" source reads 1=DREQ selected by "PERMAP" will gate source reads, 0=DREQ has no effect
				//	bit 12..15: "BURST_LENGTH" burst transfer length 1..15, in number of 32-bit words, 0=single transfer
				//	bit 16..20: "PERMAP" indicates peripheral mapping number 1..31, whose ready signal shall be used
				//			to control rate of transfers and whose panic signals will be output on DMA AXI bus.
				//			0 = continuous un-paced transfer
				//	bit 21..25: "WAITS" number of dummy wait cycles after each DMA read or write operation 0..31

	io32	SRC;		// 0x0C: source address (read-only, loaded from DMA_CB_t control block; updated by the DMA engine as the transfer progresses)

	io32	DST;		// 0x10: destination address (read-only, loaded from DMA_CB_t control block; updated by the DMA engine as the transfer progresses)

	io32	LEN;		// 0x14: transfer length (read-only, loaded from DMA_CB_t control block; updated by the DMA engine as the transfer progresses)
				//  CHANNEL 0..6:
				//	- normal mode (non 2D, bit 1 of TI is cleared):
				//			bit 0..29: amount of bytes to be transfered
				//	- 2D mode (bit 1 of TI is set):
				//			bit 0..15: "XLENGTH", length of line in bytes
				//			bit 16..29: "YLENGTH", number xlength transfers (and then stride address)
				//  CHANNEL 7..14:
				//	bit 0..15: "XLENGTH", amount of bytes to be transfered

	io32	STRIDE;		// 0x18: 2D mode stride, increment of address after transfer one xlength line (read-only, loaded from DMA_CB_t control block)
				//  CHANNEL 0..6:
				//	bit 0..15: "S_STRIDE" source stride, signed (2's complement) byte increment of source after each row in 2D mode
				//	bit 16..31: "D_STRIDE" destination stride, signed (2's complement) byte increment of destination after each row in 2D mode
				//  CHANNEL 7..14:
				//	- reserved

	io32	NEXT;		// 0x1C: address of next DMA control block DMA_CB_t (NULL = no next control block; loaded from DMA_CB_t control block)
				//	- can be changed during transfer, but DMA should be paused

	io32	DEBUG;		// 0x20: debug
				//	bit 0: "READ_LAST_NOT_SET_ERROR" read 1=last not set error (write 1 to clear)
				//	bit 1: "FIFO_ERROR" read 1=FIFO error (write 1 to clear)
				//	bit 2: "READ_ERROR" read 1=slave read response error (write 1 to clear)
				//	bit 4..7: (read only) "OUTSTANDING_WRITES" number of write responses that have not yet been received
				//	bit 8..15: (read only) "DMA_ID" DMA AXI ID of this DMA channel
				//	bit 16..24: (read only) "DMA_STATE" value of DMA engine state for this channel
				//	bit 25..27: (read only) "VERSION" DMA version, indicating control bit field changes
				//	bit 28: (read only) "LITE" set if DMA is reduced performance LITE engine (= 1 on channels 7..14)
} DMA_t;
STATIC_ASSERT(sizeof(DMA_t) == 0x24, "Incorrect DMA_t!");

// attribute of the DMA ring buffer - use coherent non-cached memory
#define DMABUF	COHERENT

// DMA channels 0..14 (do not use DMA 15, it is exclusively used by the VPU)
// DMA channels 7..14 do not support 2D transfer, cannot ignore read/write, can transfer max. 64 KB.
#define DMA(ch)		((DMA_t*)(uintptr_t)(ARM_DMA_BASE + (ch)*0x100))

// DMA interrupt status register
//	bit 0..15: (read only) interrupt status of DMA channel 0..15
#define DMA_INT_STATUS	((io32*)(uintptr_t)(ARM_DMA_BASE + 0xFE0))

// DMA enable bits register
//	bit 0..14: enable DMA channel 0..14
#define DMA_ENABLE	((io32*)(uintptr_t)(ARM_DMA_BASE + 0xFF0))

// Enable/Disable DMA channel (enabled by default)
INLINE void DMA_Enable(int ch)  { *DMA_ENABLE |= (1 << ch); }
INLINE void DMA_Disable(int ch) { *DMA_ENABLE &= ~(1 << ch); }

// check if DMA interrupt is pending
INLINE Bool DMA_IsPending(int ch) { return (*DMA_INT_STATUS & (1 << ch)) != 0; }

// clear IRQ request
INLINE void DMA_IntClr(int ch) { DMA(ch)->CS |= B1|B2; }

// reset DMA channel
INLINE void DMA_Reset(int ch) { DMA(ch)->CS = B31; }

// abort DMA transfer of current control block, DMA will continue to next control block (if any)
INLINE void DMA_Abort(int ch) { DMA(ch)->CS |= B30; }

// check if DMA transfer is in progress
INLINE Bool DMA_Check(int ch) { return (DMA(ch)->CS & B0) != 0; }

// Wait for DMA transfer to complete.
// - call DMA_End to invalidate caches after transfer
INLINE void DMA_Wait(int ch) { while (DMA_Check(ch)) {} }

// pause DMA transfer
INLINE void DMA_Pause(int ch) { DMA(ch)->CS &= ~B0; }

// continue DMA transfer
INLINE void DMA_Continue(int ch) { DMA(ch)->CS |= B0; }

// convert ARM address to BUS address
//  ARM local periphery (base address 0x40000000) cannot be accessed by the DMA.
u32 DMA_BusAddr(const void* addr);

// convert BUS address to ARM address
void* DMA_ArmAddr(u32 addr);

// Setup control block to simple memory transfer
//  cb ... DMA control block (must be 32-byte aligned)
//  dst ... destination address
//  src ... source address
//  len ... length in bytes (max. 65535 for channels 7..14)
//  inter ... raise interrupt after transfer completed
void DMA_SetupMem(DMA_CB_t* cb, void* dst, const void* src, int len, Bool inter);

// Setup control block to transfer memory window
//  cb ... DMA control block (must be 32-byte aligned)
//  dst ... destination address
//  src ... source address
//  w ... width of window, max. 65535 (length of one row in bytes)
//  h ... height of window, max. 16383 (number of rows)
//  dstpitch ... byte offset between two rows in destination buffer (can be negative, -32768+w .. +32768+w)
//  srcpitch ... byte offset between two rows in source buffer (can be negative, range -32768+w .. +32768+w)
//  inter ... raise interrupt after transfer completed
// Only DMA channels 0..6 can be used.
void DMA_SetupWin(DMA_CB_t* cb, void* dst, const void* src, int w, int h, int dstpitch, int srcpitch, Bool inter);

// Setup control block to read from IO port to memory
//  cb ... DMA control block (must be 32-byte aligned)
//  dst ... destination address (buffer of u32 values)
//  src ... source address (pointer to u32 IO port)
//  len ... number of u32 words to read (max. 16383 for channels 7..14)
//  dreq ... data request DREQ_*
//  ring ... use repeated transfer to ring buffer
//  inter ... raise interrupt after transfer completed
// Ring buffer - transfer is reset back to the beginning at the end of the buffer.
// Mark ring buffer with DMABUF attribute so that the data in the buffer
// is immediately readable without having to invalidate cache.
// ARM local periphery (base address 0x40000000) cannot be accessed by the DMA.
void DMA_SetupIORead(DMA_CB_t* cb, u32* dst, io32* src, int len, int dreq, Bool ring, Bool inter);

// Setup control block to write to IO port from memory
//  cb ... DMA control block (must be 32-byte aligned)
//  dst ... destination address (pointer to u32 IO port)
//  src ... source address (buffer of u32 values)
//  len ... number of u32 words to write (max. 16383 for channels 7..14)
//  dreq ... data request DREQ_*
//  ring ... use repeated transfer from ring buffer
//  inter ... raise interrupt after transfer completed
// Ring buffer - transfer is reset back to the beginning at the end of the buffer.
// Mark ring buffer with DMABUF attribute so that the data in the buffer
// is immediately readable without having to invalidate cache.
// ARM local periphery (base address 0x40000000) cannot be accessed by the DMA.
void DMA_SetupIOWrite(DMA_CB_t* cb, io32* dst, const u32* src, int len, int dreq, Bool ring, Bool inter);

// start DMA transfer
//  ch ... DMA channel 0..14, or use default channel DMA_CH (transfers longer than 655365 require channels 0 to 6)
// - Control block cb must stay valid until DMA transfer ends.
// - If you do not use the DMA_Setup* functions to prepare the
//   control block, you may need to invalidate the caches first.
void DMA_Start(int ch, const DMA_CB_t* cb);

// invalidate caches after transfer (not needed when transferring from memory to IO port)
void DMA_End(const DMA_CB_t* cb);

// DMA memory copy
//  ch ... DMA channel 0..14, or use default channel DMA_CH (transfers longer than 655365 require channels 0 to 6)
//  dst ... destination address
//  src ... source address
//  len ... length in bytes (max. 65535 for channels 7..14)
void DMA_MemCopyCh(int ch, void* dst, const void* src, int len);
INLINE void DMA_MemCopy(void* dst, const void* src, int len)
	{ DMA_MemCopyCh(DMA_CH, dst, src, len); }

// DMA copy window
//  ch ... DMA channel 0..6, or use default channel DMA_CH (channel 7..14 cannot be used)
//  dst ... destination address
//  src ... source address
//  w ... width of window, max. 65535 (length of one row in bytes)
//  h ... height of window, max. 16383 (number of rows)
//  dstpitch ... byte offset between two rows in destination buffer (can be negative, -32768+w .. +32768+w)
//  srcpitch ... byte offset between two rows in source buffer (can be negative, range -32768+w .. +32768+w)
void DMA_WinCopyCh(int ch, void* dst, const void* src, int w, int h, int dstpitch, int srcpitch);
INLINE void DMA_WinCopy(void* dst, const void* src, int w, int h, int dstpitch, int srcpitch)
	{ DMA_WinCopyCh(DMA_CH, dst, src, w, h, dstpitch, srcpitch); }

// DMA read from IO port to memory
//  ch ... DMA channel 0..14, or use default channel DMA_CH (transfers longer than 655365 require channels 0 to 6)
//  dst ... destination address (buffer of u32 values)
//  src ... source address (pointer to u32 IO port)
//  len ... number of u32 words to read (max. 16383 for channels 7..14)
//  dreq ... data request DREQ_*
void DMA_IOReadCh(int ch, u32* dst, io32* src, int len, int dreq);
INLINE void DMA_IORead(u32* dst, io32* src, int len, int dreq)
	{ DMA_IOReadCh(DMA_CH, dst, src, len, dreq); }

// DMA write to IO port from memory
//  ch ... DMA channel 0..14, or use default channel DMA_CH (transfers longer than 655365 require channels 0 to 6)
//  dst ... destination address (pointer to u32 IO port)
//  src ... source address (buffer of u32 values)
//  len ... number of u32 words to write (max. 16383 for channels 7..14)
//  dreq ... data request DREQ_*
void DMA_IOWriteCh(int ch, io32* dst, const u32* src, int len, int dreq);
INLINE void DMA_IOWrite(io32* dst, const u32* src, int len, int dreq)
	{ DMA_IOWriteCh(DMA_CH, dst, src, len, dreq); }

#ifdef __cplusplus
}
#endif

#endif // _SDK_DMA_H
