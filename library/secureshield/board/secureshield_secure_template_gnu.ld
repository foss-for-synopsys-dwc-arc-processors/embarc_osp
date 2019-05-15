#define __ASSEMBLY__
#include "secureshield_board_config.h"


MEMORY {
	SECURE_ROM :	ORIGIN = SECURE_ROM_START,	LENGTH = SECURE_ROM_SIZE
	SECURE_RAM :	ORIGIN = SECURE_RAM_START,	LENGTH = SECURESHIELD_SECURE_RAM_USAGE
}

ENTRY(_secureshield_start)

SECTIONS {

	.text :  ALIGN(4) {
		_f_text = .;
		KEEP(*(.secureshield_init .secureshield_init.*))
		*(.text .text.* .gnu.linkonce.t.*)
		_e_text = .;
	} > SECURE_ROM

	.rodata : ALIGN(4) {
		_f_rodata = .;

		. = ALIGN(4);
		__CTOR_LIST__ = .;
		LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
		KEEP(*(SORT_BY_NAME(".ctors*")))
		LONG(0)
		__CTOR_END__ = .;

		. = ALIGN(4);
		__init_array_start = .;
		KEEP(*(SORT_BY_NAME(".init_array*")))
		__init_array_end = .;

		. = ALIGN(4);
		__DTOR_LIST__ = .;
		LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
		KEEP(*(SORT_BY_NAME(".dtors*")))
		LONG(0)
		__DTOR_END__ = .;

		*(.rodata .rodata.* .gnu.linkonce.r.*)

#if  SECURESHIELD_VERSION == 1
		_e_rodata = _f_text + CONTAINER_SIZE_ALIGNMENT(. - _f_text);
#elif SECURESHIELD_VERSION == 2
		_e_rodata = ALIGN(CONTAINER_ADDRESS_ALIGNMENT);
#endif
	} > SECURE_ROM

	.data :  ALIGN(4) {

		_f_data = .;
		*(.data .data.* .gnu.linkonce.d.*)
		_f_sdata = .;
		__SDATA_BEGIN__ = .;
		*(.sdata .sdata.* .gnu.linkonce.s.*)
		PROVIDE (__sbss_start = .);
		PROVIDE (___sbss_start = .);
		_f_sbss = .;
		*(.dynsbss)
		*(.sbss .sbss.* .gnu.linkonce.sb.*)
		*(.scommon)
		_e_sbss = .;
		PROVIDE (__sbss_end = .);
		PROVIDE (___sbss_end = .);
		_e_sdata = .;
		_e_data = .;
	} > SECURE_RAM AT > SECURE_ROM

	.bss (NOLOAD) : ALIGN(4) {
		_f_bss = .;
		*(.dynbss)
		*(.bss .bss.* .gnu.linkonce.b.*)
		*(COMMON)
		_e_bss = .;
	} > SECURE_RAM

	.stack (NOLOAD) :
	{
		. = ALIGN(4);
		_f_stack = .;
		. = . + SECURE_STACKSIZE;
		_e_stack = .;
	} > SECURE_RAM

	.heap (NOLOAD) :
	{
		. = ALIGN(4);
		__start_heap = . ;
		_f_heap = .;
		. = . + SECURE_HEAPSIZE;
		_e_heap = .;
		__end_heap = . ;
	} > SECURE_RAM

	_load_addr_text = LOADADDR(.text);
	_load_addr_rodata = LOADADDR(.rodata);
	_load_addr_data = LOADADDR(.data);
	__secureshield_config = LOADADDR(.data) + SIZEOF(.data);
	__secureshield_stack = _e_stack;
}