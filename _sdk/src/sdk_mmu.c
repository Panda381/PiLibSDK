
// ****************************************************************************
//
//                        MMU Memory Management Unit
//
// ****************************************************************************
/*
All RAM areas are cached.
 - After writing to the video frame buffer, it is necessary to flush data cache (in DispUpdate()).
 - During DMA operations, it is necessary to flush/invalidate the data cache.
 - When sharing data between cores, it is sufficient to use the dsb() and dmb()
   barriers to ensure the correct sequence of data visibility.

After write to frame buffer:
   clean_dcache(buffer, size);

UART send buffer via DMA:
  clean_dcache(buffer, size);
  dsb sy;
  start_dma_tx(buffer, size);

UART receive data via DMA:
  start_dma_rx(buffer, size);
  wait_for_dma_complete();
  invalidate_dcache(buffer, size);
  dsb sy;

Send flag to another core:
  shared_flag = 1;
  dmb();
*/

#include "../../includes.h"	// includes

// ----------------------------------------------------------------------------
//            Zero1, Pi1, Pi2, Pi3 and Zero2 in 32-bit mode
// ----------------------------------------------------------------------------

#if (RASPPI <= 3) && (AARCH == 32)

// page table level 1 - contains 4096 entries of u32, each entry describes 1 MB of the memory (table size: 16 KB, aligned to 16 KB)
u32 __attribute__((section(".page_table_big"))) PageTableLevel1[LEVEL1_TABLE_ENTRIES];

#if RASPPI == 1
#define PAGE_ATTR_NORMAL	0x0040E		// cacheable
#else
#define PAGE_ATTR_NORMAL	0x1040E		// cacheable + shareable
#endif
// 0001 0000 0100 0000 1110
//   bit 0,1: section descriptor: 2=1 MB section
//   bit 2: B bufferable = 1 enable
//   bit 3: C cacheable = 1 enable
//   bit 4: NX = 0 executable
//   bit 5..8: domain = 0
//   bit 9: P = 0
//   bit 10,11: AP = 1 privileged R/W, user no access
//   bit 12..14: TEX = 0, with C=1 B=1: Outer and Inner Write-Back, No Allocate on Write
//   bit 15: APX = 0
//   bit 16: S shareable = 1 enable
//   bit 17: nG = 0
//   bit 18: section/supersection: 0=section
//   bit 19: NS non-secure = 0

#define PAGE_ATTR_COHERENT	0x10412		// strongly ordered
// 0001 0000 0100 0001 0110
//   bit 0,1: section descriptor: 2=1 MB section
//   bit 2: B bufferable = 0 disable
//   bit 3: C cacheable = 0 disable
//   bit 4: NX = 1 non-executable
//   bit 5..8: domain = 0
//   bit 9: P = 0
//   bit 10,11: AP = 1 privileged R/W, user no access
//   bit 12..14: TEX = 0, with C=0 B=0: strongly ordered
//   bit 15: APX = 0
//   bit 16: S shareable = 1 enable
//   bit 17: nG = 0
//   bit 18: section/supersection: 0=section
//   bit 19: NS non-secure = 0

#define PAGE_ATTR_DEVICE	0x10416		// shared device
// 0001 0000 0100 0001 0110
//   bit 0,1: section descriptor: 2=1 MB section
//   bit 2: B bufferable = 1 enable
//   bit 3: C cacheable = 0 disable
//   bit 4: NX = 1 non-executable
//   bit 5..8: domain = 0
//   bit 9: P = 0
//   bit 10,11: AP = 1 privileged R/W, user no access
//   bit 12..14: TEX = 0, with C=0 B=1: Shared Device
//   bit 15: APX = 0
//   bit 16: S shareable = 1 enable
//   bit 17: nG = 0
//   bit 18: section/supersection: 0=section
//   bit 19: NS non-secure = 0

// video area attribute
#if MMU_CACHE_MODE == 1  // MMU cache mode: 0=all cached (default), 1=video not cached, 2=no cache
#define PAGE_ATTR_VIDEO		PAGE_ATTR_DEVICE	// video non-cached
#else
#define PAGE_ATTR_VIDEO		PAGE_ATTR_NORMAL	// video cached
#endif

#define MMU_BLOCK_SIZE		0x00100000		// block size = 1 MB

// Initialize page table for Zero/Pi1/Pi2/Pi3
void PageTableInit(void)
{
	// get memory blocks
	u32 armbase = ARMMemAddr & ~(MMU_BLOCK_SIZE-1); // ARM memory base address
	u32 armend = (ARMMemAddr + ARMMemSize + MMU_BLOCK_SIZE-1) & ~(MMU_BLOCK_SIZE-1); // ARM memory end
	u32 vcbase = VCMemAddr & ~(MMU_BLOCK_SIZE-1); // VideoCore memory base address
	u32 vcend = (VCMemAddr + VCMemSize + MMU_BLOCK_SIZE-1) & ~(MMU_BLOCK_SIZE-1); // VideoCore memory end
	u32 coherent = (uintptr_t)&__coherent_start;

	// loop entries of page table
	int inx = 0;
	u32 base = 0;
	u32* page = PageTableLevel1;
	for (; inx < LEVEL1_TABLE_ENTRIES; inx++) // 4096 entries
	{
		// default access - device, non cacheable
		u32 attr = PAGE_ATTR_DEVICE;

		// coherent memory (mailbox, DMA)
		if (base == coherent)
			attr = PAGE_ATTR_COHERENT; // coherent memory - strongly ordered

		// normal access to whole ARM
		else if ((base >= armbase) && (base < armend))  // ARM memory block
			attr = PAGE_ATTR_NORMAL; // normal memory block with full access, non-cacheable

		// video VC memory (VC memory may require flush data cache)
		else if ((base >= vcbase) && (base < vcend))	// VC memory block
			attr = PAGE_ATTR_VIDEO; // video memory

		// set attribute (base address in bits 20..31)
		*page = base | attr;

		// shift to next entry (each entry addresses 1 MB of memory)
		base += MMU_BLOCK_SIZE;
		page++;
	}

	// Clean data cache
	CleanDataCache();
}

// ----------------------------------------------------------------------------
//  Pi2, Pi3 and Zero2 in 64-bit mode; Pi4 and Pi5 in 32-bit and 64-bit modes
// ----------------------------------------------------------------------------

#else // (RASPPI <= 3) && (AARCH == 32)

// page table level 1 - contains 4 entries of u64, each entry describes 1 GB of memory (table size: 32 B, aligned to 4 KB)
u64 __attribute__((section(".page_table_small"))) PageTableLevel1[LEVEL1_TABLE_ENTRIES];

// page table level 2 - contains 4 x 512 entries of u64, each entry describes 2 MB of the memory (table size: 16 KB, aligned to 4 KB)
u64 __attribute__((section(".page_table_big"))) PageTableLevel2[LEVEL1_TABLE_ENTRIES*LEVEL2_TABLE_ENTRIES];

#define PAGE_ATTR_NORMAL	(1|(0<<2)|(3<<8)|(1<<10))	// cacheable + shareable
//  bit 0-1: 1=level 2 block descriptor
//  bit 2-4: index of MAIR0 register: 0 normal
//  bit 5: NS = 0
//  bit 6-7: AP = 0, privileged R/W, user no access
//  bit 8-9: SH = 3 inner shareable
//  bit 10: AF = 1 access flag
//  bit 11: nG = 0 global
//  bit 12..20: 0
//  bit 21..39: base address
//  bit 40..51: 0
//  bit 52: continuous = 0
//  bit 53: PXN = 0 executable
//  bit 54: XN = 0 executable

#define PAGE_ATTR_COHERENT	(1|(2<<2)|(2<<8)|(1<<10)|(1ull<<53)|(1ull<<54))	// strongly ordered
//  bit 0-1: 1=level 2 block descriptor
//  bit 2-4: index of MAIR0 register: 2 coherent
//  bit 5: NS = 0
//  bit 6-7: AP = 0, privileged R/W, user no access
//  bit 8-9: SH = 2 outer shareable
//  bit 10: AF = 1 access flag
//  bit 11: nG = 0 global
//  bit 12..20: 0
//  bit 21..39: base address
//  bit 40..51: 0
//  bit 52: continuous = 0
//  bit 53: PXN = 1 privileged execute never
//  bit 54: XN = 1 user executed never

#define PAGE_ATTR_DEVICE	(1|(1<<2)|(2<<8)|(1<<10)|(1ull<<53)|(1ull<<54))	// shared device
//  bit 0,1: 1=level 2 block descriptor
//  bit 2-4: index of MAIR0 register: 1 device
//  bit 5: NS = 0
//  bit 6-7: AP = 0, privileged R/W, user no access
//  bit 8-9: SH = 2 outer shareable
//  bit 10: AF = 1 access flag
//  bit 11: nG = 0 global
//  bit 12..20: 0
//  bit 21..39: base address
//  bit 40..51: 0
//  bit 52: continuous = 0
//  bit 53: PXN = 1 privileged execute never
//  bit 54: XN = 1 user executed never

// video area attribute
#if MMU_CACHE_MODE == 1  // MMU cache mode: 0=all cached (default), 1=video not cached, 2=no cache
#define PAGE_ATTR_VIDEO		PAGE_ATTR_DEVICE	// video non-cached
#else
#define PAGE_ATTR_VIDEO		PAGE_ATTR_NORMAL	// video cached
#endif

#define MMU_BLOCK_SIZE		0x00200000		// block size = 2 MB

// Initialize page table for Pi4
void PageTableInit(void)
{
	// initialize page table level 1 
	int inx = 0;
	u64* page1 = PageTableLevel1;
	u64* page2 = PageTableLevel2;
	for (; inx < LEVEL1_TABLE_ENTRIES; inx++) // 4 entries
	{
		// write descriptor (address of page table level 2 descriptor:
		//	bits 12..39 (28 bits, lower 12 bits are cleared: mask 0xFFFFFFF000ull)
		*page1 = 3 | (u64)(uintptr_t)page2; // entry type 3 = table descriptor (L1 -> L2 pointer)

		// shift to next entry (each entry addresses 512*2MB = 1 GB of memory)
		page1++;
		page2 += LEVEL2_TABLE_ENTRIES;
	}

	// get memory blocks
	u32 armbase = ARMMemAddr & ~(MMU_BLOCK_SIZE-1); // ARM memory base address
	u32 armend = (ARMMemAddr + ARMMemSize + MMU_BLOCK_SIZE-1) & ~(MMU_BLOCK_SIZE-1); // ARM memory end
	u32 vcbase = VCMemAddr & ~(MMU_BLOCK_SIZE-1); // VideoCore memory base address
	u32 vcend = (VCMemAddr + VCMemSize + MMU_BLOCK_SIZE-1) & ~(MMU_BLOCK_SIZE-1); // VideoCore memory end
	u32 coherent = (uintptr_t)&__coherent_start;

	// initialize page table level 2
	u32 base = 0;
	page2 = PageTableLevel2;
	for (inx = 0; inx < LEVEL1_TABLE_ENTRIES*LEVEL2_TABLE_ENTRIES; inx++) // 2048 entries
	{
		// default access - device, non cacheable
		u64 attr = PAGE_ATTR_DEVICE;

		// coherent memory (mailbox, DMA)
		if (base == coherent)
			attr = PAGE_ATTR_COHERENT; // coherent memory - strongly ordered

		// normal access to whole ARM
		else if ((base >= armbase) && (base < armend))  // ARM memory block
			attr = PAGE_ATTR_NORMAL; // normal memory block with full access, non-cacheable

		// video VC memory (VC memory may require flush data cache)
		else if ((base >= vcbase) && (base < vcend))	// VC memory block
			attr = PAGE_ATTR_VIDEO; // video memory

		// set attribute (base address in bits 21..39)
		*page2 = base | attr;

		// shift to next entry (each entry addresses 2 MB of memory)
		base += MMU_BLOCK_SIZE;
		page2++;
	}

	// barriers
	dsb();
	isb();
}

#endif // (RASPPI <= 3) && (AARCH == 32)

// Check whether the memory block is (partially) cached
// - Uses default setup - checks flag MMU_CACHE_MODE and .coherent region.
Bool MemIsCached(const void* addr, size_t len)
{
	uintptr_t a = (uintptr_t)addr;

	// check .coherent region (not cacheable)
	if ((a >= (uintptr_t)&__coherent_start) &&
		(a + len <= (uintptr_t)&__coherent_end))
		return False;

	// check ARM RAM (cacheable)
	if (a < ARMMemAddr+ARMMemSize)
		return True;
	
	// check video RAM (cacheable)
#if MMU_CACHE_MODE == 0		// MMU cache mode: 0=all cached (default), 1=video not cached, 2=no cache
	if ((a < VCMemAddr+VCMemSize) && (a + len > VCMemAddr))
		return True;
#endif

	// not cacheable
	return False;
}
