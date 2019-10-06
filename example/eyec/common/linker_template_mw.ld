#define __ASSEMBLY__
#define ROM_BASE 0x0
#define ROM_SIZE (128 * 1024)
#define RAM_BASE (1 * 1024 * 1024)
#define RAM_SIZE (



#define SYS_MEM_BASE 0x0
#define SYS_MEM_SIZE (8 * 1024 * 1024)
#define M0_BASE SYS_MEM_BASE
#define M0_SIZE (1024 * 1024)
#define MC_BASE (M0_BASE + M0_SIZE)
#define MC_SIZE (1024 * 1024)
#define SHARED_SIZE (1024 * 1024)
#define SHARED_BASE (SYS_MEM_BASE + SYS_MEM_SIZE - SHARED_SIZE)

MEMORY {
#if defined(BOOTLOADER)
    REGION_SYS_MEM: ORIGIN = M0_BASE, LENGTH = M0_SIZE
#elif defined(APPL_M0)
    REGION_SYS_MEM: ORIGIN = M0_BASE, LENGTH = M0_SIZE
#elif defined(APPL_MC)
    REGION_SYS_MEM: ORIGIN = MC_BASE, LENGTH = MC_SIZE
#endif
    REGION_SHARED: ORIGIN = SHARED_BASE, LENGTH = SHARED_SIZE
}


ENTRY(_start)

SECTIONS {

	GROUP : {
		.init_bootstrap:{
			*(.init_vector .init_vector.*)
			*(.init_bootstrap .init_bootstrap.*)
		}
		.vector ALIGN(1024): {
			_f_vector = .;
			*(.vector .vector.*)
			_e_vector = .;
		}
		.text ALIGN(4): {
			_f_text = .;
			*(TYPE text)
			*(.text*)
			_e_text = .;
		}

		.rodata  ALIGN(4): {
			_f_rodata = .;

			_fctors = .;
			*(.ctors*)
			_ectors = .;
			_fdtors = .;
			*(.dtors*)
			_edtors = .;
			_feh_frame = .;
			*(.eh_frame*)
			_eeh_frame = .;

			*(TYPE lit)

			_e_rodata = .;
		}

	} > REGION_SYS_MEM


	GROUP : {
		.data ALIGN(8): {
			_f_data = .;
			_f_sdata = .;
			*(.sdata)
			*(.sbss)
			_e_sdata = .;
			*(TYPE data)
		}
		.tls ALIGN(8): {
			*(.tls*)
			_e_data = .;
		}
	} > REGION_SYS_MEM

	GROUP (NOLOAD) : {
		.bss ALIGN(8): {
			_f_bss = .;
			*(TYPE bss)
			_e_bss = .;
		}
		.stack ALIGN(4) SIZE(_STACKSIZE): {}
		.heap? ALIGN(4) SIZE(_HEAPSIZE): {}
	} > REGION_SYS_MEM

    GROUP : {
        .shared_data : {}
    } > REGION_SHARED

	_f_stack = ADDR(.stack);
	_e_stack = ADDR(.stack) + SIZEOF(.stack);
	_f_heap = ADDR(.heap);
	_e_heap = ADDR(.heap) + SIZEOF(.heap);

	_load_addr_text = LOADADDR(.text);
	_load_addr_rodata = LOADADDR(.rodata);
	_load_addr_data = LOADADDR(.data);
}
