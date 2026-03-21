
// ****************************************************************************
//
//                            DMA controller
//
// ****************************************************************************

#include "../../includes.h"	// includes

// convert ARM address to BUS address
//  ARM local periphery (base address 0x40000000) cannot be accessed by the DMA.
u32 DMA_BusAddr(const void* addr)
{
	u32 a = (u32)(uintptr_t)addr;

	// peripherals (ARM mapped)
	if ((a >= ARM_IO_BASE) && (a <= ARM_IO_END))
	{
		return a - ARM_IO_BASE + GPU_IO_BASE;
	}

	// RAM memory (including VC video memory)
	return BUS_ADDRESS(a);
}

// convert BUS address to ARM address
void* DMA_ArmAddr(u32 addr)
{
	// peripherals (BUS mapped)
	if ((addr >= GPU_IO_BASE) && (addr <= GPU_IO_BASE + (ARM_IO_END-ARM_IO_BASE)))
	{
		return (void*)(uintptr_t)(addr - GPU_IO_BASE + ARM_IO_BASE);
	}

	// RAM memory (including VC video memory)
	return ARM_ADDRESS(addr);
}

// Setup control block to simple memory transfer
//  cb ... DMA control block (must be 32-byte aligned)
//  dst ... destination address
//  src ... source address
//  len ... length in bytes (max. 65535 for channels 7..14)
//  inter ... raise interrupt after transfer completed
void DMA_SetupMem(DMA_CB_t* cb, void* dst, const void* src, int len, Bool inter)
{
	// transfer information
	u32 cfg =
		(inter ? B0 : 0) | // generate interrupt
		// B1 |		// 1=2D mode (only channels 0..6)
		// B3 |		// 1=wait for AXI write response
		B4 |		// 1=destination address incremented
		B5 |		// 1=128-bit destination width
		// B6 |		// 1=destination gated by DREQ
		// B7 |		// 1=ignore writes
		B8 |		// 1=source address incremented
		B9 |		// 1=128-bit source width
		// B10 |	// 1=source gated by DREQ
		// B11 |	// 1=ignore reads
		(DMA_BURST_MEM << 12) | // burst length
		// (dreq << 16) | // peripheral mapping
		// (0 << 21) |	// dummy wait cycles
		// B26 |	// 1=don't do wide writes
		0;
	cb->TI = cfg;

	// source address
	cb->SRC = DMA_BusAddr(src);

	// destination address
	cb->DST = DMA_BusAddr(dst);

	// data length (number of bytes)
	cb->LEN = len;

	// no stride
	cb->STRIDE = 0;

	// no next control block
	cb->NEXT = 0;

	// reserved
	cb->res[0] = 0;
	cb->res[1] = 0;

	// invalidate caches
	if (MemIsCached(src, len)) CleanAndInvalidateDataCacheRange((uintptr_t)src, len);
	if (MemIsCached(dst, len)) CleanAndInvalidateDataCacheRange((uintptr_t)dst, len);
}

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
void DMA_SetupWin(DMA_CB_t* cb, void* dst, const void* src, int w, int h, int dstpitch, int srcpitch, Bool inter)
{
	// transfer information
	u32 cfg =
		(inter ? B0 : 0) | // generate interrupt
		B1 |		// 1=2D mode (only channels 0..6)
		// B3 |		// 1=wait for AXI write response
		B4 |		// 1=destination address incremented
		B5 |		// 1=128-bit destination width
		// B6 |		// 1=destination gated by DREQ
		// B7 |		// 1=ignore writes
		B8 |		// 1=source address incremented
		B9 |		// 1=128-bit source width
		// B10 |	// 1=source gated by DREQ
		// B11 |	// 1=ignore reads
		(DMA_BURST_MEM << 12) | // burst length
		// (dreq << 16) | // peripheral mapping
		// (0 << 21) |	// dummy wait cycles
		// B26 |	// 1=don't do wide writes
		0;
	cb->TI = cfg;

	// source address
	cb->SRC = DMA_BusAddr(src);

	// destination address
	cb->DST = DMA_BusAddr(dst);

	// data length (window size)
	cb->LEN = w | (h << 16);

	// stride
	int sstride = srcpitch - w;
	int dstride = dstpitch - w;
	cb->STRIDE = (u16)sstride | ((u16)dstride << 16);

	// no next control block
	cb->NEXT = 0;

	// reserved
	cb->res[0] = 0;
	cb->res[1] = 0;

	// invalidate source cache
	if (MemIsCached(src, srcpitch*h))
	{
		intptr_t s = (intptr_t)src;
		for (; h > 0; h--)
		{
			CleanAndInvalidateDataCacheRange(s, w);
			s += srcpitch;
		}
	}

	// invalidate destination cache
	if (MemIsCached(dst, dstpitch*h))
	{
		intptr_t d = (intptr_t)dst;
		for (; h > 0; h--)
		{
			CleanAndInvalidateDataCacheRange(d, w);
			d += dstpitch;
		}
	}
}

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
void DMA_SetupIORead(DMA_CB_t* cb, u32* dst, io32* src, int len, int dreq, Bool ring, Bool inter)
{
	// transfer information
	u32 cfg =
		(inter ? B0 : 0) | // generate interrupt
		// B1 |		// 1=2D mode (only channels 0..6)
		B3 |		// 1=wait for AXI write response
		B4 |		// 1=destination address incremented
		// B5 |		// 1=128-bit destination width
		// B6 |		// 1=destination gated by DREQ
		// B7 |		// 1=ignore writes
		// B8 |		// 1=source address incremented
		// B9 |		// 1=128-bit source width
		B10 |		// 1=source gated by DREQ
		// B11 |	// 1=ignore reads
		(DMA_BURST_IO << 12) | // burst length
		(dreq << 16) |	// peripheral mapping
		// (0 << 21) |	// dummy wait cycles
		// B26 |	// 1=don't do wide writes
		0;
	cb->TI = cfg;

	// source address
	cb->SRC = DMA_BusAddr((void*)src);

	// destination address
	cb->DST = DMA_BusAddr(dst);

	// data length (number of bytes)
	len <<= 2;	// convert number of u32 words to number of bytes
	cb->LEN = len;

	// no stride
	cb->STRIDE = 0;

	// no next control block, or repeat transfer to ring buffer
	cb->NEXT = ring ? DMA_BusAddr(cb) : 0;

	// reserved
	cb->res[0] = 0;
	cb->res[1] = 0;

	// invalidate cache
	if (MemIsCached(dst, len)) CleanAndInvalidateDataCacheRange((uintptr_t)dst, len);
}

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
void DMA_SetupIOWrite(DMA_CB_t* cb, io32* dst, const u32* src, int len, int dreq, Bool ring, Bool inter)
{
	// transfer information
	u32 cfg =
		(inter ? B0 : 0) | // generate interrupt
		// B1 |		// 1=2D mode (only channels 0..6)
		B3 |		// 1=wait for AXI write response
		// B4 |		// 1=destination address incremented
		// B5 |		// 1=128-bit destination width
		B6 |		// 1=destination gated by DREQ
		// B7 |		// 1=ignore writes
		B8 |		// 1=source address incremented
		// B9 |		// 1=128-bit source width
		// B10 |	// 1=source gated by DREQ
		// B11 |	// 1=ignore reads
		(DMA_BURST_IO << 12) | // burst length
		(dreq << 16) |	// peripheral mapping
		// (0 << 21) |	// dummy wait cycles
		// B26 |	// 1=don't do wide writes
		0;
	cb->TI = cfg;

	// source address
	cb->SRC = DMA_BusAddr(src);

	// destination address
	cb->DST = DMA_BusAddr((void*)dst);

	// data length (number of bytes)
	len <<= 2;	// convert number of u32 words to number of bytes
	cb->LEN = len;

	// no stride
	cb->STRIDE = 0;

	// no next control block, or repeat transfer to ring buffer
	cb->NEXT = ring ? DMA_BusAddr(cb) : 0;

	// reserved
	cb->res[0] = 0;
	cb->res[1] = 0;

	// invalidate cache
	if (MemIsCached(src, len)) CleanAndInvalidateDataCacheRange((uintptr_t)src, len);
}

// start DMA transfer
//  ch ... DMA channel 0..14, or use default channel DMA_CH (transfers longer than 655365 require channels 0 to 6)
// - Control block cb must stay valid until DMA transfer ends.
// - If you do not use the DMA_Setup* functions to prepare the
//   control block, you may need to invalidate the caches first.
void DMA_Start(int ch, const DMA_CB_t* cb)
{
	// invalidate cache with control block
	CleanAndInvalidateDataCacheRange((uintptr_t)cb, sizeof(DMA_CB_t));

	// prepare DMA channel address
	DMA_t* dma = DMA(ch);

	// set address of control block
	dma->CB = DMA_BusAddr(cb);

	// start transfer
	dma->CS = B0 |				// "ACTIVE", start transfer
		(DMA_NORM_PRIORITY << 16) |	// normal priority
		(DMA_PANIC_PRIORITY << 20) |	// panic priority
		B28;				// wait for outstanding AXI writes
}

// invalidate caches after transfer (not needed when transferring from memory to IO port)
void DMA_End(const DMA_CB_t* cb)
{
	// get configuration
	u32 cfg = cb->TI;

	// destination is RAM if incremented
	if ((cfg & B4) != 0)
	{
		// destination address
		uintptr_t d = (uintptr_t)DMA_ArmAddr(cb->DST);

		// data length (number of bytes)
		u32 len = cb->LEN;

		// linear transfer if not 2D
		if ((cfg & B1) == 0)
		{
			// invalidate cache
			if (MemIsCached((void*)d, len)) CleanAndInvalidateDataCacheRange(d, len);
		}

		// 2D windows transfer
		else
		{
			// get width and height
			int w = len & 0xffff;
			int h = (len >> 16) & 0x3fff;

			// get destination pitch
			int dstpitch = ((s32)cb->STRIDE >> 16) + w;

			// invalidate destination cache
			if (MemIsCached((void*)d, dstpitch*h))
			{
				for (; h > 0; h--)
				{
					CleanAndInvalidateDataCacheRange(d, w);
					d += dstpitch;
				}
			}
		}
	}
}

// DMA memory copy
//  ch ... DMA channel 0..14, or use default channel DMA_CH (transfers longer than 655365 require channels 0 to 6)
//  dst ... destination address
//  src ... source address
//  len ... length in bytes (max. 65535 for channels 7..14)
void DMA_MemCopyCh(int ch, void* dst, const void* src, int len)
{
	// reset DMA channel
	DMA_Reset(ch);

	// enable DMA channel
	DMA_Enable(ch);

	// prepare memory control block
	DMA_CB_t cb;
	DMA_SetupMem(&cb, dst, src, len, False);

	// start DMA transfer
	DMA_Start(ch, &cb);

	// Wait for DMA transfer to complete.
	DMA_Wait(ch);
	
	// invalidate caches after transfer
	DMA_End(&cb);
}

// DMA copy window
//  ch ... DMA channel 0..6, or use default channel DMA_CH (channel 7..14 cannot be used)
//  dst ... destination address
//  src ... source address
//  w ... width of window, max. 65535 (length of one row in bytes)
//  h ... height of window, max. 16383 (number of rows)
//  dstpitch ... byte offset between two rows in destination buffer (can be negative, -32768+w .. +32768+w)
//  srcpitch ... byte offset between two rows in source buffer (can be negative, range -32768+w .. +32768+w)
void DMA_WinCopyCh(int ch, void* dst, const void* src, int w, int h, int dstpitch, int srcpitch)
{
	// reset DMA channel
	DMA_Reset(ch);

	// enable DMA channel
	DMA_Enable(ch);

	// prepare memory control block
	DMA_CB_t cb;
	DMA_SetupWin(&cb, dst, src, w, h, dstpitch, srcpitch, False);

	// start DMA transfer
	DMA_Start(ch, &cb);

	// Wait for DMA transfer to complete.
	DMA_Wait(ch);
	
	// invalidate caches after transfer
	DMA_End(&cb);
}

// DMA read from IO port to memory
//  ch ... DMA channel 0..14, or use default channel DMA_CH (transfers longer than 655365 require channels 0 to 6)
//  dst ... destination address (buffer of u32 values)
//  src ... source address (pointer to u32 IO port)
//  len ... number of u32 words to read (max. 16383 for channels 7..14)
//  dreq ... data request DREQ_*
void DMA_IOReadCh(int ch, u32* dst, io32* src, int len, int dreq)
{
	// reset DMA channel
	DMA_Reset(ch);

	// enable DMA channel
	DMA_Enable(ch);

	// prepare memory control block
	DMA_CB_t cb;
	DMA_SetupIORead(&cb, dst, src, len, dreq, False, False);

	// start DMA transfer
	DMA_Start(ch, &cb);

	// Wait for DMA transfer to complete.
	DMA_Wait(ch);
	
	// invalidate caches after transfer
	DMA_End(&cb);
}

// DMA write to IO port from memory
//  ch ... DMA channel 0..14, or use default channel DMA_CH (transfers longer than 655365 require channels 0 to 6)
//  dst ... destination address (pointer to u32 IO port)
//  src ... source address (buffer of u32 values)
//  len ... number of u32 words to write (max. 16383 for channels 7..14)
//  dreq ... data request DREQ_*
void DMA_IOWriteCh(int ch, io32* dst, const u32* src, int len, int dreq)
{
	// reset DMA channel
	DMA_Reset(ch);

	// enable DMA channel
	DMA_Enable(ch);

	// prepare memory control block
	DMA_CB_t cb;
	DMA_SetupIOWrite(&cb, dst, src, len, dreq, False, False);

	// start DMA transfer
	DMA_Start(ch, &cb);

	// Wait for DMA transfer to complete.
	DMA_Wait(ch);
}
