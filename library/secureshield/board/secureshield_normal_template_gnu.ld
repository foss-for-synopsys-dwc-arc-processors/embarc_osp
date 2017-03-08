#define __ASSEMBLY__
#include "secureshield_board_config.h"

MEMORY {
	NORMAL_ROM :	ORIGIN = NORMAL_ROM_START,	LENGTH = NORMAL_ROM_SIZE
	SECURE_ROM :	ORIGIN = SECURE_ROM_START,	LENGTH = SECURE_ROM_SIZE
	NORMAL_RAM :	ORIGIN = NORMAL_RAM_START,	LENGTH = NORMAL_RAM_SIZE
	SECURE_RAM :	ORIGIN = SECURE_RAM_START,	LENGTH = SECURE_RAM_SIZE
}

ENTRY(_start)

SECTIONS
{
	. = ORIGIN(SECURE_ROM);
	.rom.secureshield ALIGN(CONTAINER_ADDRESS_ALIGNMENT) : {
		/* secureshield code and rodata */
		. = ALIGN(4);
		_f_rom_secureshieldruntime = .;
		KEEP(*(.rom.secureshield.secure))

		/* the following are secureshield application configuration */
		_f_cfg_secureshield = .;
		/* secureshield container configuration tables */
		. = ALIGN(32);
		__secureshield_cfgtbl_start = .;
		KEEP(*(.keep.secureshield.cfgtbl))
		. = ALIGN(32);
		__secureshield_cfgtbl_end = .;

		/* pointers to secureshield secure shields configuration tables */
		/* note: no further alignment here, we need to have the exact list of pointers */
		__secureshield_cfgtbl_ptr_start = .;
		KEEP(*(.keep.secureshield.cfgtbl_ptr_first))
		KEEP(*(.keep.secureshield.cfgtbl_ptr))
		__secureshield_cfgtbl_ptr_end = .;

		KEEP(*(.keep.secureshield.ac_table))

		. = ALIGN(32);

		_e_cfg_secureshield = .;

#if  SECURESHIELD_VERSION == 1
		_e_rom_secureshieldruntime = _f_rom_secureshieldruntime + CONTAINER_SIZE_ALIGNMENT(. - _f_rom_secureshieldruntime);
#elif SECURESHIELD_VERSION == 2
		. = ALIGN(CONTAINER_ADDRESS_ALIGNMENT);
		_e_rom_secureshieldruntime = .;
#endif
	} > SECURE_ROM

#ifdef SECURESHIELD_REGION_SECURE_CONTAINERS_ROM
#if  SECURESHIELD_VERSION == 1
	.rom.gap.secure BIND(_e_rom_secureshieldruntime - 4) : {
		LONG(0)
	} > SECURE_ROM
#endif
	SECURESHIELD_REGION_SECURE_CONTAINERS_ROM
#endif

	. = ORIGIN(SECURE_RAM);
	.ram.secureshield ALIGN(CONTAINER_ADDRESS_ALIGNMENT) : {
		/* secureshield runtime data */
		. = ALIGN(4);
		_f_ram_secureshieldruntime = .;

		KEEP(*(.ram.secureshield.secure))

#if  SECURESHIELD_VERSION == 1
		_e_ram_secureshieldruntime = _f_ram_secureshieldruntime + CONTAINER_SIZE_ALIGNMENT(. - _f_ram_secureshieldruntime);
#elif SECURESHIELD_VERSION == 2
		. = ALIGN(CONTAINER_ADDRESS_ALIGNMENT);
		_e_ram_secureshieldruntime = .;
#endif
	} > SECURE_RAM

#ifdef SECURESHIELD_REGION_SECURE_CONTAINERS_RAM
#if  SECURESHIELD_VERSION == 1
	.ram.gap.secure BIND(_e_ram_secureshieldruntime - 4) : {
		LONG(0)
	} > SECURE_RAM
#endif
	SECURESHIELD_REGION_SECURE_CONTAINERS_RAM
#endif

	. = ORIGIN(NORMAL_ROM);

	.init :
	{
		_f_text_background_container = .;
		_f_init = .;
		KEEP (*(.init_vector))
		KEEP (*(.init_bootstrap))
		_e_init = .;
	} > NORMAL_ROM

	.vector : ALIGN(1024)
	{
		_f_vector = .;
		*(.vector)
		_e_vector = .;
	} > NORMAL_ROM

	.text : ALIGN(4)
	{
		_f_text = .;
		*(EXCLUDE_FILE(CONTAINER_ROM_OBJS_LIST) .text EXCLUDE_FILE(CONTAINER_ROM_OBJS_LIST) .text.* EXCLUDE_FILE(CONTAINER_ROM_OBJS_LIST) .gnu.linkonce.t.*)
		_e_text = .;
	} > NORMAL_ROM

#if SECURESHIELD_VERSION == 2
	.rodata : ALIGN(CONTAINER_ADDRESS_ALIGNMENT)
	{
#else
	.rodata : ALIGN(4)
	{
#endif
		_e_text_background_container = .;
		_f_rodata_background_container = .;
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

		*(EXCLUDE_FILE(CONTAINER_ROM_OBJS_LIST) .rodata EXCLUDE_FILE(CONTAINER_ROM_OBJS_LIST) .rodata.* EXCLUDE_FILE(CONTAINER_ROM_OBJS_LIST) .gnu.linkonce.r.*)
		_e_rodata = .;
	} > NORMAL_ROM
#if  SECURESHIELD_VERSION == 1
		_e_rodata_background_container = _f_text_background_container + CONTAINER_SIZE_ALIGNMENT(. - _f_text_background_container);
#elif SECURESHIELD_VERSION == 2
		_e_rodata_background_container = ALIGN(CONTAINER_ADDRESS_ALIGNMENT);
#endif

#ifdef SECURESHIELD_REGION_CONTAINERS_ROM
#if  SECURESHIELD_VERSION == 1
	.rom.gap BIND(_e_rodata_background_container - 4) : {
		LONG(0)
	} > NORMAL_ROM
#endif
	SECURESHIELD_REGION_CONTAINERS_ROM
#endif	

	. = ORIGIN(NORMAL_RAM);

	.data : ALIGN(4)
	{	_f_data_background_container = .;
		_f_data = .;
		*(EXCLUDE_FILE(CONTAINER_RAM_OBJS_LIST) .data EXCLUDE_FILE(CONTAINER_RAM_OBJS_LIST) .data.* EXCLUDE_FILE(CONTAINER_RAM_OBJS_LIST) .gnu.linkonce.d.*)
		_f_sdata = .;
		__SDATA_BEGIN__ = .;
		*(EXCLUDE_FILE(CONTAINER_RAM_OBJS_LIST) .sdata EXCLUDE_FILE(CONTAINER_RAM_OBJS_LIST) .sdata.* EXCLUDE_FILE(CONTAINER_RAM_OBJS_LIST) .gnu.linkonce.s.*)
		_e_sdata = .;
		_e_data = .;
		_e_data_background_container = .;
	} > NORMAL_RAM AT > NORMAL_ROM


	.bss (NOLOAD) : ALIGN(4)
	{
		_f_bss_background_container = .;
		PROVIDE (__sbss_start = .);
		PROVIDE (___sbss_start = .);
		_f_bss = .;
		_f_sbss = .;
		*(EXCLUDE_FILE(CONTAINER_RAM_OBJS_LIST) .dynsbss)
		*(EXCLUDE_FILE(CONTAINER_RAM_OBJS_LIST) .sbss EXCLUDE_FILE(CONTAINER_RAM_OBJS_LIST) .sbss.*  EXCLUDE_FILE(CONTAINER_RAM_OBJS_LIST) .gnu.linkonce.sb.*)
		*(EXCLUDE_FILE(CONTAINER_RAM_OBJS_LIST) .scommon)
		_e_sbss = .;
		PROVIDE (__sbss_end = .);
		PROVIDE (___sbss_end = .);
		*(EXCLUDE_FILE(CONTAINER_RAM_OBJS_LIST) .dynbss)
		*(EXCLUDE_FILE(CONTAINER_RAM_OBJS_LIST) .bss EXCLUDE_FILE(CONTAINER_RAM_OBJS_LIST) .bss.* EXCLUDE_FILE(CONTAINER_RAM_OBJS_LIST) .gnu.linkonce.b.*)
		*( COMMON)
		_e_bss = .;
	} > NORMAL_RAM

	.stack (NOLOAD) :
	{
		. = ALIGN(4);
		_f_stack = .;
		. = . + _STACKSIZE;
		_e_stack = .;
	} > NORMAL_RAM

	.heap (NOLOAD) :
	{
		. = ALIGN(4);
		__start_heap = . ;
		_f_heap = .;
		. = . + _HEAPSIZE;
		_e_heap = .;
		__end_heap = . ;
	} > NORMAL_RAM
#if  SECURESHIELD_VERSION == 1
		_e_bss_background_container = _f_data_background_container + CONTAINER_SIZE_ALIGNMENT(. - _f_data_background_container);
#elif SECURESHIELD_VERSION == 2
		_e_bss_background_container = ALIGN(CONTAINER_ADDRESS_ALIGNMENT);
#endif

#ifdef SECURESHIELD_REGION_CONTAINERS_RAM
#if  SECURESHIELD_VERSION == 1
	.ram.gap BIND(_e_bss_background_container - 4) : {
		LONG(0)
	} > NORMAL_RAM
#endif
	SECURESHIELD_REGION_CONTAINERS_RAM

#endif
	_load_addr_text = LOADADDR(.text);
	_load_addr_rodata = LOADADDR(.rodata);
	_load_addr_data = LOADADDR(.data);
	_f_data_load_background_container = LOADADDR(.data);
}
