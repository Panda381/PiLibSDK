
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
*/

#ifndef _SDK_MMU_H
#define _SDK_MMU_H

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------------
//            Zero1, Pi1, Pi2, Pi3 and Zero2 in 32-bit mode
// ----------------------------------------------------------------------------

#if (RASPPI <= 3) && (AARCH == 32)

#define LEVEL1_TABLE_ENTRIES	4096		// number of entries of page table level 1
// page table level 1 - contains 4096 entries of u32, each entry describes 1 MB of the memory (table size: 16 KB, aligned to 16 KB)
extern u32 __attribute__((section(".page_table_big"))) PageTableLevel1[LEVEL1_TABLE_ENTRIES];

// ----------------------------------------------------------------------------
//  Pi2, Pi3 and Zero2 in 64-bit mode; Pi4 and Pi5 in 32-bit and 64-bit modes
// ----------------------------------------------------------------------------

#else // (RASPPI <= 3) && (AARCH == 32)

#define LEVEL1_TABLE_ENTRIES	4		// number of entries of page table level 1
#define LEVEL2_TABLE_ENTRIES	512		// number of entries of page table level 2
// page table level 1 - contains 4 entries of u64, each entry describes 1 GB of memory (table size: 32 B, aligned to 4 KB)
extern u64 __attribute__((section(".page_table_small"))) PageTableLevel1[LEVEL1_TABLE_ENTRIES];

// page table level 2 - contains 4 x 512 entries of u64, each entry describes 2 MB of the memory (table size: 16 KB, aligned to 4 KB)
extern u64 __attribute__((section(".page_table_big"))) PageTableLevel2[LEVEL1_TABLE_ENTRIES*LEVEL2_TABLE_ENTRIES];

#endif // (RASPPI <= 3) && (AARCH == 32)

// Initialize page table
void PageTableInit(void);

// Enable MMU (this function is located in startup32.S and startup64.S)
void EnableMMU(void);

// Disable MMU (this function is located in startup32.S and startup64.S)
void DisableMMU(void);

// Check whether the memory block is (partially) cached
// - Uses default setup - checks flag MMU_CACHE_MODE and .coherent region.
Bool MemIsCached(const void* addr, size_t len);

#ifdef __cplusplus
}
#endif

#endif // _SDK_MMU_H
