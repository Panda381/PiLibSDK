
// ****************************************************************************
//
//                          Memory allocator
//
// ****************************************************************************
// The memory allocator is inter-core safe, can be shared across multiple CPU
// cores, but must not be used inside IRQ/FIQ interrupt handlers.
// Typical allocation speed: small blocks (up to 256 KB) around 1 us,
// large blocks (over 256 KB) units to tens of microseconds.

#if USE_MALLOC		// 1=use memory allocator, 0=not used (lib_malloc.*)

#ifndef _LIB_MALLOC_H
#define _LIB_MALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

#define MEM_PAGE_SHIFT	18			// order of page size (18 = 256 KB)
#define MEM_PAGE_SIZE	(1<<MEM_PAGE_SHIFT)	// page size
#define MEM_PAGE_NUM	(0xffffffff/MEM_PAGE_SIZE+1) // number of pages

// page type
#define MEM_PAGE_FREE	0		// page is free
#define MEM_PAGE_RES	1		// page is reserved (page is free, but blocks it contains are in list of free blocks)
#define MEM_PAGE_LONG	2		// page is part of long block (> MEM_PAGE_SIZE)
#define MEM_PAGE_MODMIN	3		// minimal page module (= 8 bytes)
#define MEM_PAGE_MODMAX	MEM_PAGE_SHIFT	// maximal page module (= MEM_PAGE_SIZE)
#define MEM_PAGE_MODNUM	(MEM_PAGE_MODMAX-MEM_PAGE_MODMIN+1) // number of modules
#define MEM_PAGE_INV	0xff		// invalid page (no RAM)

// page descriptor
typedef struct {
	u16	used;		// number of used blocks in this page, or number of following pages of long block
	u8	module;		// type if this page - flags MEM_PAGE_* or module in range MEM_PAGE_MODMIN to MEM_PAGE_MODMAX
	u8	res;		// ... reserved (align)
} sMemPageDesc;
STATIC_ASSERT(sizeof(sMemPageDesc) == 4, "Incorrect sMemPageDesc!");

// module descriptor
typedef struct {
	sList	freeblock;	// list of free blocks of this module
	u16	respage;	// index of reserved page or 0 = none (reserved page is free, but blocks it contains are in list of free blocks)
	u8	module;		// module size MEM_PAGE_MODMIN..MEM_PAGE_MODMAX (block size = 1<<module)
} sMemModDesc;

// memory allocator descriptor
typedef struct {
	// memory info
	u32	membase;	// memory base
	u32	memsize;	// memory size
	u32	memfree;	// free memory in bytes
	u32	memused;	// used allocated memory in bytes

#if CORES > 1
	// memory allocator lock
	volatile u8 lock;	// memory allocator lock
#endif

	// list of free pages (each free page contains an sListEntry item at the beginning)
	sList	freepage;

	// module descriptors
	sMemModDesc	mod[MEM_PAGE_MODNUM];	// module descriptors

	// page descriptors (size 65536 bytes)
	sMemPageDesc	page[MEM_PAGE_NUM];	// page descriptors

} sMemAllocDesc;

extern sMemAllocDesc MemAllocDesc;	// memory allocator descriptor

// get total size of allocable memory
INLINE u32 MemGetTotal(void) { return MemAllocDesc.memsize; }

// get size of free memory in bytes
INLINE u32 MemGetFree(void) { return MemAllocDesc.memfree; }

// get size of used memory in bytes
INLINE u32 MemGetUsed(void) { return MemAllocDesc.memused; }

// get max. size of allocable large block (large block is multiply of MEM_PAGE_SIZE)
size_t MemGetMax(void);

// Initialize memory allocator
void MemInit(void);

// allocate memory block (returns NULL on memory error)
void* MemAlloc(size_t size);
void* malloc(size_t size);

// free memory block (pointer can be NULL)
void MemFree(void* addr);
void free(void* addr);

// get memory block size (performs some checks on the validity of the block and returns 0 if the block
// is not valid - however, it cannot perform a guaranteed check, it is only an approximate check)
size_t MemSize(void* addr);

// reallocate memory block
void* MemResize(void* addr, size_t size);
void* realloc(void* addr, size_t size);

/*
#ifdef __cplusplus
// inplace "new" operator
//		example:	new (&m_List[inx]) CText;
//					m_List[i].~CText()
//INLINE void* operator new (size_t size, void* p) { size; return p; }
//INLINE void operator delete (void* adr, void* p) { adr; p; return; }
INLINE void *operator new(size_t n) { return malloc(n); }
INLINE void *operator new[](size_t n) { return malloc(n); }
INLINE void operator delete(void *p) noexcept { free(p); }
INLINE void operator delete[](void *p) noexcept { free(p); }
INLINE void operator delete(void *p, __unused size_t n) noexcept { free(p); }
INLINE void operator delete[](void *p, __unused size_t n) noexcept { free(p); }
#endif // __cplusplus
*/

#ifdef __cplusplus
}
#endif

#endif // _LIB_MALLOC_H

#endif // USE_MALLOC
