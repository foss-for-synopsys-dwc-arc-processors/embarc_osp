#define __ASSEMBLY__
#include "secureshield_board_config.h"


MEMORY {
	SECURE_ROM :	ORIGIN = SECURE_ROM_START,	LENGTH = SECURE_ROM_SIZE
	SECURE_RAM :	ORIGIN = SECURE_RAM_START,	LENGTH = SECURESHIELD_SECURE_RAM_USAGE
}

ENTRY(_secureshield_start)

SECTIONS {
	GROUP : {

		.text ALIGN(4): {
			_f_text = .;
			KEEP(*(.secureshield_init .secureshield_init.*))
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
#if  SECURESHIELD_VERSION == 1
			_e_rodata = _f_text + CONTAINER_SIZE_ALIGNMENT(. - _f_text);
#elif SECURESHIELD_VERSION == 2
			_e_rodata = ALIGN(CONTAINER_ADDRESS_ALIGNMENT);
#endif
		}

	} > SECURE_ROM


	GROUP : {
		.data ALIGN(4): {
			_f_data = .;
			_f_sdata = .;
			*(.sdata)
			*(.sbss)
			_e_sdata = .;
			*(TYPE data)
		}
		.tls ALIGN(4): {
			*(.tls*)
			_e_data = .;
		}
	} > SECURE_RAM AT > SECURE_ROM

	GROUP (NOLOAD) : {
		.bss ALIGN(4): {
			_f_bss = .;
			*(TYPE bss)
			_e_bss = .;
		}
		.stack ALIGN(4) SIZE(SECURE_STACKSIZE): {}
		.heap? ALIGN(4) SIZE(SECURE_HEAPSIZE): {}
	} > SECURE_RAM

	_f_stack = ADDR(.stack);
	_e_stack = ADDR(.stack) + SIZEOF(.stack);
	__secureshield_stack = _e_stack;
	_f_heap = ADDR(.heap);
	_e_heap = ADDR(.heap) + SIZEOF(.heap);

	__secureshield_config = LOADADDR(.tls) + SIZEOF(.tls);

	_load_addr_text = LOADADDR(.text);
	_load_addr_rodata = LOADADDR(.rodata);
	_load_addr_data = LOADADDR(.data);
}