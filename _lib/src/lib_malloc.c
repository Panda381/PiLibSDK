
// ****************************************************************************
//
//                          Memory allocator
//
// ****************************************************************************

#include "../../includes.h"	// includes

#if USE_MALLOC		// 1=use memory allocator, 0=not used (lib_malloc.*)

sMemAllocDesc MemAllocDesc;	// memory allocator descriptor

extern u8 __heap_start__;	// start of the heap

// lock (SpinLocks are not supported on non-cached memory)
#if (CORES > 1) && USE_MULTICORE && (MMU_CACHE_MODE != 2)
#define MEM_LOCK()	SpinLockLock(&m->lock)
#define MEM_UNLOCK()	SpinLockUnlock(&m->lock);
#else
#define MEM_LOCK()
#define MEM_UNLOCK()
#endif

// allocate new free page, mark it as new reserve page, add new blocks (returns False on memory error)
// - this function does not lock memory allocator
static Bool MemNewPage(sMemModDesc* md);

// Initialize memory allocator
void MemInit(void)
{
	int i;

	// pointer to the descriptor of memory allocator
	sMemAllocDesc* m = &MemAllocDesc;

#if CORES > 1
	// initialize memory allocator lock
	SpinLockInit(&m->lock);
#endif

	// get memory base and memory size
	u32 base = (u32)(uintptr_t)&__heap_start__;	// base address of the heap
	base = (base + MEM_PAGE_SIZE-1) & ~(MEM_PAGE_SIZE-1); // round base address up
	u32 end = ARMMemAddr + ARMMemSize; 		// end of ARM memory
	end = end & ~(MEM_PAGE_SIZE-1);			// round end address down
	u32 size = end - base;				// size of allocable memory

	// setup memory info
	m->membase = base;				// memory base
	m->memsize = size;				// memory size
	m->memfree = size;				// free memory in bytes
	m->memused = 0;					// used allocated memory in bytes

	// initialize list of free pages
	ListInit(&m->freepage);

	// initialize list of pages
	sMemPageDesc* p = m->page;			// list of page descriptors
	u32 a = 0;					// address of the page
	for (i = MEM_PAGE_NUM; i > 0; i--)
	{
		// mark this page as invalid
		p->module = MEM_PAGE_INV;		// this page is invalid

		// check if this page is valid
		if ((a >= base) && (a < end))
		{
			// pointer to page content
			sListEntry* pa = (sListEntry*)(uintptr_t)a;	// pointer to page content

			// add page to list of free pages
			ListAddLast(&m->freepage, pa);		// add page to list of free pages

			// mark this page as free
			p->module = MEM_PAGE_FREE;		// mark this page as free
		}

		// shift to next page
		p++;					// next page descriptor
		a += MEM_PAGE_SIZE;			// shift address of the page
	}

	// initialize list of module descriptors
	sMemModDesc* md = m->mod;			// list of module descriptors
	int module = MEM_PAGE_MODMIN;
	for (i = MEM_PAGE_MODNUM; i > 0; i--)
	{
		// initialize list of free blocks
		ListInit(&md->freeblock);

		// no reserved page
		md->respage = 0;

		// set module size
		md->module = module;
		module++;

		// next module descriptor
		md++;
	}

	// allocate some new pages, because the first allocation of
	// small blocks may cause a delay in user's program execution
	MemNewPage(&m->mod[0]);		// 8 bytes per block
	MemNewPage(&m->mod[1]);		// 16 bytes per block
	MemNewPage(&m->mod[2]);		// 32 bytes per block
	MemNewPage(&m->mod[3]);		// 64 bytes per block
	MemNewPage(&m->mod[4]);		// 128 bytes per block
}

// allocate new free page, mark it as new reserve page, add new blocks (returns False on memory error)
// - this function does not lock memory allocator
static Bool MemNewPage(sMemModDesc* md)
{
	// pointer to the descriptor of memory allocator
	sMemAllocDesc* m = &MemAllocDesc;

	// check if list of free pages is empty
	if (ListIsEmpty(&m->freepage)) return False;

	// get next page from the list
	sListEntry* pa = ListGetFirst(&m->freepage);
	ListEntryRemove(pa);

	// get index of the page
	u32 a = (u32)(uintptr_t)pa;		// page address
	int inx = a >> MEM_PAGE_SHIFT;		// get page index
	md->respage = inx;			// use this page as reserved page

	// mark the page as free
	sMemPageDesc* p = &m->page[inx];	// get page descriptor
	p->used = 0;				// no used blocks
	int module = md->module;		// module of the blocks
	p->module = module;			// setup type of the page

	// add blocks to the list
	int size = 1 << module;			// size of the block in bytes
	int off = 0;				// offset of the block
	for (; off < MEM_PAGE_SIZE; off += size) // loop through all blocks
	{
		// address of the block
		sListEntry* b = (sListEntry*)(uintptr_t)(a + off);

		// add block to the list of free blocks
		ListAddLast(&md->freeblock, b);
	}
	return True;
}

// allocate memory block (returns NULL on memory error)
void* MemAlloc(size_t size)
{
	// pointer to the descriptor of memory allocator
	sMemAllocDesc* m = &MemAllocDesc;

	// fast check size of required block
	if ((size == 0) || (size > (size_t)m->memfree)) return NULL;

	// lock memory allocator
	MEM_LOCK();

	// small block is required
	if (size <= MEM_PAGE_SIZE)
	{
		// prepare module index
		int module = Order(size-1);
		if (module < MEM_PAGE_MODMIN) module = MEM_PAGE_MODMIN;

		// get pointer to module descriptor
		sMemModDesc* md = &m->mod[module-MEM_PAGE_MODMIN];

		// check if list of free blocks is empty
		if (ListIsEmpty(&md->freeblock))
		{
			// allocate new free page, mark it as new reserve page, add new blocks (returns False on memory error)
			if (!MemNewPage(md))
			{
				// unlock memory allocator
				MEM_UNLOCK();
				return NULL;
			}
		}

		// get next free block
		sListEntry* b = ListGetFirst(&md->freeblock);

		// remove block from the list of free blocks
		ListEntryRemove(b);

		// get page index and address from block address
		int inx = (u32)(uintptr_t)b >> MEM_PAGE_SHIFT; // page index
		u32 a = inx << MEM_PAGE_SHIFT;	// page address

		// delete reserve page, if it is this page
		if (inx == md->respage) md->respage = 0; // reserved page will no longer be a free page

		// increase number of used blocks in the page and set module of the page
		sMemPageDesc* p = &m->page[inx];	// get page descriptor
		p->used++;				// increment number of used blocks

		// add size of this block to the statistics
		int size = 1 << module;			// size of the block in bytes
		m->memfree -= size;			// decrease number of free bytes
		m->memused += size;			// increase number of used bytes

		// unlock memory allocator
		MEM_UNLOCK();
		return (void*)b;
	}

	// large block is required, prepare required number of pages
	int num = (size + MEM_PAGE_SIZE-1) >> MEM_PAGE_SHIFT;

	// search for a sequence of free pages of the required size
	int i, j, mx;
	sMemPageDesc* p = m->page;
	mx = ((m->membase + m->memsize) >> MEM_PAGE_SHIFT) - num; // max. index to search for (without length of the block)
	for (i = 0; i <= mx; i++)
	{
		// check if this page is free
		if (p->module == MEM_PAGE_FREE)
		{
			// check length of this sequence
			for (j = 1; j < num; j++)
			{
				if (p[j].module != MEM_PAGE_FREE) break;
			}

			// correct page sequence has been found
			if (j == num)
			{
				// update size
				u32 size = num << MEM_PAGE_SHIFT;
				m->memfree -= size;	// decrease number of free bytes
				m->memused += size;	// increase number of used bytes

				// get pages
				j = i; // index of this block
				for (; num > 0;)
				{
					// number of remaining pages
					num--;

					// mark this page as large block
					p->module = MEM_PAGE_LONG; // this is large block
					p->used = num;	// number of remaining blocks

					// release block from list of free blocks
					sListEntry* pa = (sListEntry*)(uintptr_t)(j << MEM_PAGE_SHIFT);
					ListEntryRemove(pa);

					// next block
					j++;
					p++;
				}

				// unlock memory allocator
				MEM_UNLOCK();

				// return address of the block
				return (void*)(uintptr_t)(i << MEM_PAGE_SHIFT);
			}
		}

		// shift to next page descriptor
		p++;
	}

	// unlock memory allocator
	MEM_UNLOCK();

	// memory error
	return NULL;
}

// release reserved page
// - this function does not lock memory allocator
static void MemDelPage(sMemModDesc* md)
{
	// check if reserved page is valid
	int inx = md->respage;			// get reserved page
	if (inx == 0) return;			// reserved page is not valid
	md->respage = 0;			// invalidate reserved page

	// pointer to the descriptor of memory allocator
	sMemAllocDesc* m = &MemAllocDesc;

	// address of the page
	u32 a = inx << MEM_PAGE_SHIFT;		// address of the page
	int module = md->module;		// module of the blocks

	// remove blocks from the list
	int size = 1 << module;			// size of the block in bytes
	int off = 0;				// offset of the block
	for (; off < MEM_PAGE_SIZE; off += size) // loop through all blocks
	{
		// address of the block
		sListEntry* b = (sListEntry*)(uintptr_t)(a + off);

		// remove block from the list of free blocks
		ListEntryRemove(b);
	}

	// add page to the list of free pages
	sListEntry* pa = (sListEntry*)(uintptr_t)a; // pointer to page content
	ListAddLast(&m->freepage, pa);		// add page to list of free pages

	// get descriptor of the page
	sMemPageDesc* p = &m->page[inx];	// get page descriptor

	// mark this page as free
	p->module = MEM_PAGE_FREE;		// mark this page as free
}

// free memory block (pointer can be NULL)
void MemFree(void* addr)
{
	// base check validity of the block
	size_t size = MemSize(addr);
	if (size == 0) return;

	// pointer to the descriptor of memory allocator
	sMemAllocDesc* m = &MemAllocDesc;

	// get page address and check if address of the block is in valid range
	u32 inx = (u32)(uintptr_t)addr >> MEM_PAGE_SHIFT; // page index
	u32 a = inx << MEM_PAGE_SHIFT;			// page address

	// lock memory allocator
	MEM_LOCK();

	// get page descriptor and module
	sMemPageDesc* p = &m->page[inx];
	int module = p->module;

	// large block
	if (module == MEM_PAGE_LONG)
	{
		// get number of blocks
		u32 num = p->used + 1;

		// update size
		u32 size = num << MEM_PAGE_SHIFT;
		m->memfree += size;	// increase number of free bytes
		m->memused -= size;	// decrease number of used bytes

		// delete pages
		for (; num > 0; num--)
		{
			// add page to the list of free pages
			sListEntry* pa = (sListEntry*)(uintptr_t)a; // pointer to page content
			ListAddLast(&m->freepage, pa);	// add page to list of free pages

			// mark this page as free
			p->module = MEM_PAGE_FREE;	// mark this page as free

			// next page
			inx++;			// index of next page
			a += MEM_PAGE_SIZE;	// address of next page
			p++;			// descriptor of next page
		}
	}

	// small block
	else
	{
		// update size
		u32 size = 1 << module;
		m->memfree += size;	// increase number of free bytes
		m->memused -= size;	// decrease number of used bytes

		// get pointer to module descriptor
		sMemModDesc* md = &m->mod[module-MEM_PAGE_MODMIN];

		// add this block to list of free blocks
		ListAddLast(&md->freeblock, (sListEntry*)addr);

		// decrement number of used blocks
		p->used--;

		// it was last block of the page - set this page as reserved page
		if (p->used == 0)
		{
			// delete reserved page
			MemDelPage(md);

			// set new reserved page
			md->respage = inx;
		}
	}

	// unlock memory allocator
	MEM_UNLOCK();
}

// get memory block size (performs some checks on the validity of the block and returns 0 if the block
// is not valid - however, it cannot perform a guaranteed check, it is only an approximate check)
size_t MemSize(void* addr)
{
	// NULL has size 0
	if (addr == NULL) return 0;

	// pointer to the descriptor of memory allocator
	sMemAllocDesc* m = &MemAllocDesc;

	// get page address and check if address of the block is in valid range
	u32 inx = (u32)(uintptr_t)addr >> MEM_PAGE_SHIFT; // page index
	u32 a = inx << MEM_PAGE_SHIFT;			// page address
	if ((a < m->membase) || (a >= m->membase + m->memsize)) return 0;

	// lock memory allocator
	MEM_LOCK();

	// get page descriptor and module
	sMemPageDesc* p = &m->page[inx];
	int module = p->module;

	// large block
	if (module == MEM_PAGE_LONG)
	{
		// check align of block address
		if (((u32)(uintptr_t)addr & (MEM_PAGE_SIZE-1)) != 0)
		{
			// unlock memory allocator
			MEM_UNLOCK();
			return 0;
		}

		// check number of blocks
		u32 num = p->used + 1;
		MEM_UNLOCK();
		u32 mx = ((m->membase + m->memsize) >> MEM_PAGE_SHIFT); // number of valid blocks
		if (inx + num > mx) return 0;	// invalid size of large block

		// get size of large block
		return num << MEM_PAGE_SHIFT;
	}

	// unlock memory allocator
	MEM_UNLOCK();

	// check validity of modules of the page
	if ((module < MEM_PAGE_MODMIN) || (module > MEM_PAGE_MODMAX)) return 0; // invalid module

	// check align of block address
	u32 size = 1 << module;
	if (((u32)(uintptr_t)addr & (size-1)) != 0) return 0;
	return size;
}

// reallocate memory block
void* MemResize(void* addr, size_t size)
{
	// address is NULL - allocate new memory
	if (addr == NULL)
	{
		return MemAlloc(size);
	}

	// size 0 - release block
	if (size == 0)
	{
		MemFree(addr);
		return NULL;
	}

	// get old size of the block
	size_t oldsize = MemSize(addr);

	// old size 0 = invalid old block
	if (oldsize == 0) return NULL;

	// check required size of the block
	sMemAllocDesc* m = &MemAllocDesc;
	if (size > (size_t)m->memsize) return NULL;

	// prepare new size of the block
	size_t newsize;
	if (size > MEM_PAGE_SIZE)
	{
		// large block
		newsize = (size + MEM_PAGE_SIZE-1) & ~(MEM_PAGE_SIZE-1);
	}
	else
	{
		// small block
		int module = Order(size-1);
		if (module < MEM_PAGE_MODMIN) module = MEM_PAGE_MODMIN;
		newsize = 1 << module;
	}

	// size has not been changed
	if (newsize == oldsize) return addr;

	// allocate new memory block
	void* newaddr = MemAlloc(size);
	if (newaddr == NULL) return NULL;

	// copy data
	if (size >= oldsize)
		memcpy(newaddr, addr, oldsize);
	else
		memcpy(newaddr, addr, size);

	// delete old memory block
	MemFree(addr);
	return newaddr;
}

// get max. size of allocable large block (large block is multiply of MEM_PAGE_SIZE)
size_t MemGetMax(void)
{
	// pointer to the descriptor of memory allocator
	sMemAllocDesc* m = &MemAllocDesc;

	// lock memory allocator
	MEM_LOCK();

	// loop through pages
	u32 maxsize = 0;
	sMemPageDesc* p = m->page;
	int i, j;
	u32 mx = ((m->membase + m->memsize) >> MEM_PAGE_SHIFT); // number of valid blocks
	for (i = 0; i < mx; i++)
	{
		// check if this page is free
		if (p[i].module == MEM_PAGE_FREE)
		{
			u32 size = 0;
			for (j = i; j < mx; j++)
			{
				if (p[j].module != MEM_PAGE_FREE) break;
				size += MEM_PAGE_SIZE;
			}
			if (size > maxsize) maxsize = size;

			// continue after this non-free page
			i = j;
		}
	}

	// unlock memory allocator
	MEM_UNLOCK();
	return maxsize;
}

// allocate memory block (returns NULL on memory error)
void* WRAPPER_FUNC(malloc)(size_t size)
{
	return MemAlloc(size);
}

// free memory block (pointer can be NULL)
void WRAPPER_FUNC(free)(void* addr)
{
	MemFree(addr);
}

// reallocate memory block
void* WRAPPER_FUNC(realloc)(void* addr, size_t size)
{
	return MemResize(addr, size);
}

#endif // USE_MALLOC
