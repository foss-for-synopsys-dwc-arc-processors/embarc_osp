#define __ASSEMBLY__
#include <target_mem_config.h>

#define SYS_MEM_BASE 0x0
#define SYS_MEM_SIZE (8 * 1024 * 1024)
#define M0_BASE SYS_MEM_BASE
#define M0_SIZE (1024 * 1024)
#define MC_BASE (M0_BASE + M0_SIZE)
#define MC_SIZE (1024 * 1024)

MEMORY {
#if defined(APPL_M0)
    REGION_SYS_MEM: ORIGIN = M0_BASE, LENGTH = M0_SIZE
#elif defined(APPL_MC)
    REGION_SYS_MEM: ORIGIN = MC_BASE, LENGTH = MC_SIZE
#endif
}

#if 0
MEMORY {
#if (REGION_ICCM_SIZE != 0)
	REGION_ICCM :		ORIGIN = REGION_ICCM_START,	LENGTH = REGION_ICCM_SIZE
#endif
#if (REGION_DCCM_SIZE !=0)
	REGION_DCCM :		ORIGIN = REGION_DCCM_START,	LENGTH = REGION_DCCM_SIZE
#endif
#if (REGION_EXT_ROM_SIZE != 0)
	REGION_EXT_ROM :	ORIGIN = REGION_EXT_ROM_START,	LENGTH = REGION_EXT_ROM_SIZE
#endif
#if (REGION_EXT_RAM_SIZE != 0)
	REGION_EXT_RAM :	ORIGIN = REGION_EXT_RAM_START,	LENGTH = REGION_EXT_RAM_SIZE
#endif
}
#endif

ENTRY(_start)

SECTIONS {

#if 0
	GROUP : {
		.image_head: {
			. = . + IMAGE_HEAD_SIZE;
		}
		.init_bootstrap:{
			*(.init_vector .init_vector.*)
			*(.init_bootstrap .init_bootstrap.*)
		}
		.vector ALIGN(1024): {
			_f_vector = .;
			*(.vector .vector.*)
			_e_vector = .;
		}
	} > REGION_ROM
#endif

	GROUP : {
		.image_head: {
			. = . + IMAGE_HEAD_SIZE;
		}
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

	_f_stack = ADDR(.stack);
	_e_stack = ADDR(.stack) + SIZEOF(.stack);
	_f_heap = ADDR(.heap);
	_e_heap = ADDR(.heap) + SIZEOF(.heap);

	_load_addr_text = LOADADDR(.text);
	_load_addr_rodata = LOADADDR(.rodata);
	_load_addr_data = LOADADDR(.data);
}
