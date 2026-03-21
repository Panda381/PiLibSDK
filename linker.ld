/* Linker script */

/* entry point */
ENTRY( _start )

SECTIONS {

	/* .init section, start code */
	.init LOAD_ADDR : {
		KEEP(*(.init))
	}

	/* .text section, program code */
	.text : {
		. = ALIGN(16);
		*(.text*)
		_etext = .;
	}

	/* .rodata section, initialized read-only data */
	.rodata : {
		. = ALIGN(16);
		*(.rodata*)
	}

	/* static constructors of static objects, initialize arrays */
	.init_array : {
		. = ALIGN(16);
		__init_start = .;
		KEEP(*(.init_array*))
		__init_end = .;
	}

	/* exception table to unwind stack */
	.ARM.exidx : {
		. = ALIGN(16);
		__exidx_start = .;
		*(.ARM.exidx*)
		__exidx_end = .;
	}

	/* exception frames to unwind stack */
	.eh_frame : {
		. = ALIGN(16);
		*(.eh_frame*)
	}

	/* .data section, initialized data */
	.data : {
		. = ALIGN(16);
		*(.data*)
	}

	/* .bss section, uninitialized data */
	.bss : {
		. = ALIGN(16);
		__bss_start = .;
		*(.bss*)
		*(COMMON)
		__bss_end = .;
		_end = .;
	}

	/* stack */
	.stack (NOLOAD) : {
		. = ALIGN(16);
		__stack_start = .;
		. = . + (CORES*CORE_STACK_SIZE);
		__stack_end = .;
	}

	/* MMU table section */
	.page_table (NOLOAD) : {
		. = ALIGN(16384);	/* 16 KB alignment */
		*(.page_table_big)
		. = ALIGN(4096);
		*(.page_table_small)
		. = ALIGN(16);
	}

	/* 2 MB coherent region - mailbox, DMA */
	.coherent (NOLOAD) : {
		. = ALIGN(2097152);	/* 2 MB alignment */
		__coherent_start = .;
		*(.coherent)
		__coherent_end = .;
		. = ALIGN(2097152);	/* 2 MB alignment */
	}

	/* .heap section */
	.heap : {
		. = ALIGN(2097152);	/* 2 MB alignment */
		__heap_start__ = .;
	}
}
