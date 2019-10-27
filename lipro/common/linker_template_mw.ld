#define __ASSEMBLY__
#include "lipro_memmap.h"

MEMORY {
#if defined(BOOTLOADER)
    REGION_CODE: ORIGIN = BL_CODE_START, LENGTH = BL_CODE_SIZE
    REGION_DATA: ORIGIN = BL_DATA_START, LENGTH = BL_DATA_SIZE
#elif defined(APPL_M0)
    REGION_CODE: ORIGIN = MP_CODE_START, LENGTH = MP_CODE_SIZE
    REGION_DATA: ORIGIN = MP_DATA_START, LENGTH = MP_DATA_SIZE
#elif defined(APPL_MC)
    REGION_CODE: ORIGIN = MC_CODE_START, LENGTH = MC_CODE_SIZE
    REGION_DATA: ORIGIN = MC_DATA_START, LENGTH = MC_DATA_SIZE
#elif defined(APPL_SECOND_STAGE)
    REGION_CODE: ORIGIN = SSTG_CODE_START, LENGTH = SSTG_CODE_SIZE
    REGION_DATA: ORIGIN = SSTG_DATA_START, LENGTH = SSTG_DATA_SIZE
    #undef _STACKSIZE
    #define _STACKSIZE 4096
#endif
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
		.tls ALIGN(8): {
			*(.tls*)
			_e_data = .;
		} 
                .sdata: { 
			_f_sdata = .;
			/**(.sdata)
			*(.sbss)*/
			_e_sdata = .;                    
                }
		.data ALIGN(8): {
			_f_data = .;
			*(TYPE data)
		}
        } > REGION_CODE

	GROUP (NOLOAD) : {
		.bss ALIGN(8): {
			_f_bss = .;
			*(TYPE bss)
			_e_bss = .;
		}
		.stack ALIGN(4) SIZE(_STACKSIZE): {}
		.heap? ALIGN(4) SIZE(_HEAPSIZE): {}
	} > REGION_DATA


        _shared_data_addr = SHARED_DATA_START;
        _second_stage_addr = SSTG_CODE_START;
	_f_stack = ADDR(.stack);
	_e_stack = ADDR(.stack) + SIZEOF(.stack);
	_f_heap = ADDR(.heap);
	_e_heap = ADDR(.heap) + SIZEOF(.heap);

	_load_addr_text = LOADADDR(.text);
	_load_addr_rodata = LOADADDR(.rodata);
	_load_addr_data = LOADADDR(.data);
}
