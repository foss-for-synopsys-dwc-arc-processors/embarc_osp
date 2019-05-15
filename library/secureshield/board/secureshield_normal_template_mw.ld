#define __ASSEMBLY__
#include "secureshield_board_config.h"

MEMORY {
	NORMAL_ROM :	ORIGIN = NORMAL_ROM_START,	LENGTH = NORMAL_ROM_SIZE
	SECURE_ROM :	ORIGIN = SECURE_ROM_START,	LENGTH = SECURE_ROM_SIZE
	NORMAL_RAM :	ORIGIN = NORMAL_RAM_START,	LENGTH = NORMAL_RAM_SIZE
	SECURE_RAM :	ORIGIN = SECURE_RAM_START,	LENGTH = SECURE_RAM_SIZE
}

ENTRY(_start)

SECTIONS {

	GROUP: {
		.rom.secureshieldruntime ALIGN(CONTAINER_ADDRESS_ALIGNMENT) : {
			/* secureshield secure code, rodata, data */
			. = ALIGN(4);
			_f_rom_secureshieldruntime = .;
			KEEP(*(.rom.secureshield.secure .rom.secureshield.secure.*))


			/* the following are secureshield application configuration */
			_f_cfg_secureshield = .;
			/* secureshield container configuration tables */
			. = ALIGN(32);
			__secureshield_cfgtbl_start = .;
			KEEP(*(.keep.secureshield.cfgtbl .keep.secureshield.cfgtbl.*))
			. = ALIGN(32);
			__secureshield_cfgtbl_end = .;

			/* pointers to secureshield secure shields configuration tables */
			/* note: no further alignment here, we need to have the exact list of pointers */
			 __secureshield_cfgtbl_ptr_start = .;
			KEEP(*(.keep.secureshield.cfgtbl_ptr_first .keep.secureshield.cfgtbl_ptr_first.*))
			KEEP(*(.keep.secureshield.cfgtbl_ptr .keep.secureshield.cfgtbl_ptr.*))
			 __secureshield_cfgtbl_ptr_end = .;

			KEEP(*(.keep.secureshield.ac_table .keep.secureshield.ac_table.*))

			. = ALIGN(32);
			_e_cfg_secureshield = .;
#if  SECURESHIELD_VERSION == 1
			_e_rom_secureshieldruntime = _f_rom_secureshieldruntime + CONTAINER_SIZE_ALIGNMENT(. - _f_rom_secureshieldruntime);
#elif SECURESHIELD_VERSION == 2
			_e_rom_secureshieldruntime = ALIGN(CONTAINER_ADDRESS_ALIGNMENT);
#endif
		}
	} > SECURE_ROM

#ifdef SECURESHIELD_REGION_SECURE_CONTAINERS_ROM
#if  SECURESHIELD_VERSION == 1
	.rom.gap.secure BIND(_e_rom_secureshieldruntime-4) : {
		FILL(4);
	} > SECURE_ROM
#endif
	SECURESHIELD_REGION_SECURE_CONTAINERS_ROM
#endif

	GROUP (NOLOAD) : {
		.ram.secureshieldruntime ALIGN(CONTAINER_ADDRESS_ALIGNMENT) : {
			/* secureshield runtime data */
			. = ALIGN(4);
			_f_ram_secureshieldruntime = .;
			KEEP(*(.ram.secureshield.secure .ram.secureshield.secure.*))
			KEEP(*(.ram.secureshield.stack .ram.secureshield.stack.*))
#if  SECURESHIELD_VERSION == 1
			_e_ram_secureshieldruntime = _f_ram_secureshieldruntime + CONTAINER_SIZE_ALIGNMENT(. - _f_ram_secureshieldruntime);
#elif SECURESHIELD_VERSION == 2
			_e_ram_secureshieldruntime  = ALIGN(CONTAINER_ADDRESS_ALIGNMENT);
#endif
		}


	} > SECURE_RAM

#ifdef SECURESHIELD_REGION_SECURE_CONTAINERS_RAM
#if  SECURESHIELD_VERSION == 1
	.ram.gap.secure BIND(_e_ram_secureshieldruntime-4) : {
		FILL(4);
	} > SECURE_RAM
#endif
	SECURESHIELD_REGION_SECURE_CONTAINERS_RAM
#endif

/* background container's area */
	GROUP : {
		.init_bootstrap:{
			_f_text_background_container = .;
			_f_init = .;
			KEEP(*(.init_vector .init_vector.*))
			KEEP(*(.init_bootstrap .init_bootstrap.*))
			_e_init = .;
		}

		.vector ALIGN(1024): {
			_f_vector = .;
			*(.vector .vector.*)
			_e_vector = .;
		}

		/* . = ALIGN(SECURESHIELD_CONTAINER_ALIGNMENT); */
		.text ALIGN(4): {
			_f_text = .;
			*(TYPE text)
			*(.text*)
			_e_text = .;
		}

#if SECURESHIELD_VERSION == 2
		.rodata ALIGN(CONTAINER_ADDRESS_ALIGNMENT): {
#else
		.rodata ALIGN(4): {
#endif
			_e_text_background_container = .;
			_f_rodata_background_container = .;
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
#if  SECURESHIELD_VERSION == 1
		_e_rodata_background_container = _f_text_background_container + CONTAINER_SIZE_ALIGNMENT(. - _f_text_background_container);
#elif SECURESHIELD_VERSION == 2
		_e_rodata_background_container = ALIGN(CONTAINER_ADDRESS_ALIGNMENT);
#endif

	} > NORMAL_ROM

#ifdef SECURESHIELD_REGION_CONTAINERS_ROM
#if  SECURESHIELD_VERSION == 1
	.rom.gap BIND(_e_rodata_background_container-4) : {
		FILL(4);
	} > NORMAL_ROM
#endif
	SECURESHIELD_REGION_CONTAINERS_ROM
#endif

	GROUP : {
		.data ALIGN(4): {
			_f_data_background_container = .;
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
			_e_data_background_container = .;
		}
	} > NORMAL_RAM AT > NORMAL_ROM

	GROUP (NOLOAD) : {

		.bss ALIGN(4): {
			_f_bss_background_container = .;
			_f_bss = .;
			*(TYPE bss)
			_e_bss = .;
		}
		.stack ALIGN(4) SIZE(_STACKSIZE): {}
		.heap  ALIGN(4) SIZE(_HEAPSIZE): {
		}
#if  SECURESHIELD_VERSION == 1
		_e_bss_background_container = _f_data_background_container + CONTAINER_SIZE_ALIGNMENT(. - _f_data_background_container);
#elif SECURESHIELD_VERSION == 2
		_e_bss_background_container = ALIGN(CONTAINER_ADDRESS_ALIGNMENT);
#endif
	} > NORMAL_RAM


#ifdef SECURESHIELD_REGION_CONTAINERS_RAM
#if  SECURESHIELD_VERSION == 1
	.ram.gap BIND(_e_bss_background_container-4) : {
		FILL(4);
	} > NORMAL_RAM
#endif
	SECURESHIELD_REGION_CONTAINERS_RAM

#endif

	_f_stack = ADDR(.stack);
	_e_stack = ADDR(.stack) + SIZEOF(.stack);
	_f_heap = ADDR(.heap);
	_e_heap = ADDR(.heap) + SIZEOF(.heap);

	_load_addr_text = LOADADDR(.text);
	_load_addr_rodata = LOADADDR(.rodata);
	_load_addr_data = LOADADDR(.data);
	_f_data_load_background_container = LOADADDR(.data);
}
