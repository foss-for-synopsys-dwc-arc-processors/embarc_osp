/*------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
--------------------------------------------- */

#ifndef _CONTAINER_CONFIG_H_
#define _CONTAINER_CONFIG_H_

#include "code_gen_macros.h"

#ifndef __ASSEMBLY__

/* background container config macro */
#define SECURESHIELD_CONTAINER_BACKGROUND(ac_table) \
	__SECURESHIELD_CONTAINER_BACKGROUND(ac_table, EMBARC_ARRAY_COUNT(ac_table))

#define __SECURESHIELD_CONTAINER_BACKGROUND(ac_table, ac_table_count) \
	extern uint8_t _e_text_background_container[]; \
	extern uint8_t _f_text_background_container[]; \
	extern uint8_t _e_rodata_background_container[]; \
	extern uint8_t _f_rodata_background_container[]; \
	extern uint8_t _e_bss_background_container[]; \
	extern uint8_t _f_bss_background_container[]; \
	extern uint8_t _f_data_background_container[]; \
	extern uint8_t _e_data_background_container[]; \
	extern uint8_t _f_data_load_background_container[]; \
	uint32_t __attribute__((section(".ram.secureshield.stack"), aligned(4))) \
		secure_stack_background_container[CONTAINER_SECURE_STACK_SIZE]; \
	static const __attribute__((section(".keep.secureshield.cfgtbl"), aligned(4))) CONTAINER_CONFIG background_container_cfg = { \
		"background_container", \
		SECURESHIELD_CONTAINER_MAGIC, \
		SECURESHIELD_CONTAINER_NORMAL, \
		secure_stack_background_container + CONTAINER_SECURE_STACK_SIZE, \
		0,			\
		0,			\
		ac_table, \
		ac_table_count, \
		_f_text_background_container, \
		_e_text_background_container, \
		_f_rodata_background_container, \
		_e_rodata_background_container, \
		_f_data_background_container, \
		_e_data_background_container, \
		_f_data_load_background_container,\
		_f_bss_background_container,\
		_e_bss_background_container \
	}; \
	\
	const __attribute__((section(".keep.secureshield.cfgtbl_ptr_first"), aligned(4))) \
	void * const background_container_ptr = &background_container_cfg;

/* this macro selects an overloaded macro (variable number of arguments) */
#define __SECURESHIELD_CONTAINER_MACRO(_1, _2, _3, _4, NAME, ...) NAME

/* normal container config macro */
#define __SECURESHIELD_NORMAL_CONTAINER_CONFIG(container_name, ac_table, stack_size) \
	\
	extern uint8_t _e_text_ ## container_name[]; \
	extern uint8_t _f_text_ ## container_name[]; \
	extern uint8_t _e_rodata_ ## container_name[]; \
	extern uint8_t _f_rodata_ ## container_name[]; \
	extern uint8_t _e_bss_ ## container_name[]; \
	extern uint8_t _f_bss_ ## container_name[]; \
	extern uint8_t _f_data_ ## container_name[]; \
	extern uint8_t _e_data_ ## container_name[]; \
	extern uint8_t _f_data_load_ ## container_name[]; \
	uint32_t __attribute__((section(".ram.secureshield.stack"), aligned(4))) \
		secure_stack_ ## container_name[CONTAINER_SECURE_STACK_SIZE]; \
	uint32_t __attribute__((section(EMBARC_TO_STRING(.container_name.bss)))) \
		stack_ ## container_name[(stack_size+3)/4]; \
	\
	static const __attribute__((section(".keep.secureshield.cfgtbl"), aligned(4))) CONTAINER_CONFIG container_name ## _cfg = { \
		EMBARC_TO_STRING(container_name), \
		SECURESHIELD_CONTAINER_MAGIC, \
		SECURESHIELD_CONTAINER_NORMAL, \
		secure_stack_ ## container_name + CONTAINER_SECURE_STACK_SIZE, \
		(stack_size+3)/4, \
		stack_ ## container_name + (stack_size+3)/4, \
		ac_table, \
		EMBARC_ARRAY_COUNT(ac_table), \
		_f_text_ ## container_name, \
		_e_text_ ## container_name, \
		_f_rodata_ ## container_name, \
		_e_rodata_ ## container_name,  \
		_f_data_ ## container_name, \
		_e_data_ ## container_name, \
		_f_data_load_ ## container_name, \
		_f_bss_ ## container_name,  \
		_e_bss_ ## container_name   \
	}; \
	\
	const __attribute__((section(".keep.secureshield.cfgtbl_ptr"), aligned(4))) void * const container_name ## _cfg_ptr = &container_name ## _cfg;


/* secure container config macro */
#define __SECURESHIELD_SECURE_CONTAINER_CONFIG(container_name, ac_table, stack_size) \
	\
	extern uint8_t _e_text_ ## container_name[]; \
	extern uint8_t _f_text_ ## container_name[]; \
	extern uint8_t _e_rodata_ ## container_name[]; \
	extern uint8_t _f_rodata_ ## container_name[]; \
	extern uint8_t _e_bss_ ## container_name[]; \
	extern uint8_t _f_bss_ ## container_name[]; \
	extern uint8_t _f_data_ ## container_name[]; \
	extern uint8_t _e_data_ ## container_name[]; \
	extern uint8_t _f_data_load_ ## container_name[]; \
	uint32_t __attribute__((section(EMBARC_TO_STRING(.container_name.bss)))) \
		stack_ ## container_name[(stack_size+3)/4]; \
	static const __attribute__((section(".keep.secureshield.cfgtbl"), aligned(4))) CONTAINER_CONFIG container_name ## _cfg = { \
		EMBARC_TO_STRING(container_name), \
		SECURESHIELD_CONTAINER_MAGIC, \
		SECURESHIELD_CONTAINER_SECURE, \
		0, \
		(stack_size+3)/4, \
		stack_ ## container_name + (stack_size+3)/4, \
		ac_table, \
		EMBARC_ARRAY_COUNT(ac_table), \
		_f_text_ ## container_name, \
		_e_text_ ## container_name, \
		_f_rodata_ ## container_name, \
		_e_rodata_ ## container_name,  \
		_f_data_ ## container_name, \
		_e_data_ ## container_name, \
		_f_data_load_ ## container_name, \
		_f_bss_ ## container_name,  \
		_e_bss_ ## container_name   \
	}; \
	\
	const __attribute__((section(".keep.secureshield.cfgtbl_ptr"), aligned(4))) void * const container_name ## _cfg_ptr = &container_name ## _cfg;



#define __SECURESHIELD_CONTAINER_CONFIG_NOSTACK(container_name, ac_table) \
	__SECURESHIELD_NORMAL_CONTAINER_CONFIG(container_name, ac_table, CONTAINER_DEFAULT_STACK_SIZE)

#define __SECURESHIELD_CONTAINER_CONFIG_STACK(container_name, ac_table, stack_size) \
	__SECURESHIELD_NORMAL_CONTAINER_CONFIG(container_name, ac_table, stack_size)

#define __SECURESHIELD_CONTIANER_CONFIG_TYPE(container_name, ac_table, stack_size, container_type) \
	__SECURESHIELD_SECURE_CONTAINER_CONFIG(container_name, ac_table, stack_size)

#define CONTAINER_AC_TABLE const __attribute__((section(".keep.secureshield.ac_table"), aligned(4))) CONTAINER_AC_ITEM

#define SECURESHIELD_CONTAINER_CONFIG(...) \
	__SECURESHIELD_CONTAINER_MACRO(__VA_ARGS__, __SECURESHIELD_CONTIANER_CONFIG_TYPE, \
		__SECURESHIELD_CONTAINER_CONFIG_STACK, \
		__SECURESHIELD_CONTAINER_CONFIG_NOSTACK)(__VA_ARGS__)


#define CONTAINER_TEXT(container_name) \
		__attribute__((section(EMBARC_TO_STRING(.container_name.text))))
#define CONTAINER_RODATA(container_name) \
		__attribute__((section(EMBARC_TO_STRING(.container_name.rodata))))
#define CONTAINER_DATA(container_name) \
		__attribute__((section(EMBARC_TO_STRING(.container_name.data))))
#define CONTAINER_BSS(container_name) \
		__attribute__((section(EMBARC_TO_STRING(.container_name.bss))))

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif	/* not define __ASSEMBLY__ */

#if defined(__MW__)
#define OBJ_TEXT(obj_name)	#obj_name(TYPE text)
#define OBJ_RODATA(obj_name)	#obj_name(TYPE lit)
#define OBJ_DATA(obj_name)	#obj_name(TYPE data)
#define OBJ_BSS(obj_name)	#obj_name(TYPE BSS)
#define CONTAINER_SIZE_ALIGNMENT(x) (1 << (((x)<=2048)?11:(((x)<=4096)?12:(((x)<=8192)?\
	13:(((x)<=16384)?14:(((x)<=32768)?15:(((x)<=65536)?\
	16:(((x)<=131072)?17:(((x)<=262144)?18:(((x)<=524288)?\
	19:(((x)<=1048576)?20:(((x)<=2097152)?21:(((x)<=4194304)?\
	22:(((x)<=8388608)?23:(((x)<=16777216)?24:(((x)<=33554432)?\
	25:(((x)<=67108864)?26:(((x)<=134217728)?27:(((x)<=268435456)?\
	28:(((x)<=536870912)?29:(((x)<=1073741824)?30:(((x)<=2147483648)?\
	31:32))))))))))))))))))))))
#elif defined(__GNU__)
#define OBJ_TEXT(obj_name)	#obj_name(.text .text.* .gnu.linkonce.t.*)
#define OBJ_RODATA(obj_name)	#obj_name(.rodata .rodata.* .gnu.linkonce.r.*)
#define OBJ_DATA(obj_name)	#obj_name(.data .data.* .gnu.linkonce.d.*)
#define OBJ_BSS(obj_name)	#obj_name(.bss .bss.* .gnu.linkonce.b.*)
#define CONTAINER_SIZE_ALIGNMENT(x) (1 << LOG2CEIL(x))
#endif


#define OBJS_TEXT(...)		EVAL(MAP(OBJ_TEXT, __VA_ARGS__))
#define OBJS_RODATA(...)	EVAL(MAP(OBJ_RODATA, __VA_ARGS__))
#define	OBJS_DATA(...)		EVAL(MAP(OBJ_DATA, __VA_ARGS__))
#define	OBJS_BSS(...)		EVAL(MAP(OBJ_BSS, __VA_ARGS__))


/*
 * This file is used to tell secureshield how to generate the linker file.
 * If a memory region is required to be created for a specific container. The macro
 * 	GEN_CONTAINER_RAM_SECTION(container_name/region_name, region_size, object files if required)
 * should be called. THe macro
 * 	SECURESHIELD_REGION_CONTAINERS_ROM is the collect of definitions to generate the normal container's
 * 	sections in normal ROM. For example
 *  #define SECURESHIELD_REGION_CONTAINERS_ROM  GEN_CONTAINER_ROM_SECTION(container1, 2048)\
 *  						GEN_CONTAINER_ROM_SECTION(container2, 2048) \
 *  						GEN_CONTAINER_ROM_SECTION(container3, 2048, test1.o, test2.o)
 *  means to create sections for container 1, container 2 and container 3, and their size is 2048 bytes.
 *  For container 3, the .text and .rodata of test1.o and test2.o will also be put into container 3's rom section.
 *  Because the limitation of MPU in Secureshield V1, the possible value of region_size must be 2048, 4096, 8192,
 *  16K, 32K, 64K ... etc..
 *  The macro SECURESHIELD_REGION_CONTAINERS_RAM works the same like SECURESHIELD_REGION_CONTAINERS_ROM, it has
 *  the definitions in normal RAM
 *
 *  The macro SECURESHIELD_REGION_SECURE_CONTAINER_ROM and SECURESHIELD_REGION_SECURE_CONTAINER_RAM are used for
 *  secure ROM and secure RAM
 */

#if SECURESHIELD_VERSION == 1

#define GEN_CONTAINER_ROM_SECTION(container_name, size, ...) \
		.container_name.rom ALIGN(CONTAINER_SIZE_ALIGNMENT(size)) : { \
		_f_text_##container_name = .;		\
		*(.container_name.text)			\
		IF_ELSE(HAS_ARGS(__VA_ARGS__))(		\
			OBJS_TEXT(__VA_ARGS__)		\
		)()					\
		_e_text_##container_name = .;		\
		_f_rodata_##container_name = .; 	\
		*(.container_name.rodata)		\
		IF_ELSE(HAS_ARGS(__VA_ARGS__))(		\
			OBJS_RODATA(__VA_ARGS__)	\
		)()					\
		_e_rodata_##container_name = ALIGN(CONTAINER_SIZE_ALIGNMENT(size)); \
		} > NORMAL_ROM

#define GEN_CONTAINER_RAM_SECTION(container_name, size, ...) \
		.container_name.ram.data ALIGN(CONTAINER_SIZE_ALIGNMENT(size)) : { \
		_f_data_##container_name = .;		\
		*(.container_name.data)			\
		IF_ELSE(HAS_ARGS(__VA_ARGS__))(		\
			OBJS_DATA(__VA_ARGS__)		\
		)()					\
		_e_data_##container_name = .;		\
		} > NORMAL_RAM AT > NORMAL_ROM		\
		.container_name.ram.bss : {		\
		_f_bss_##container_name = .;		\
		*(.container_name.bss)			\
		IF_ELSE(HAS_ARGS(__VA_ARGS__))(		\
			OBJS_BSS(__VA_ARGS__)		\
		)()					\
		_e_bss_##container_name = ALIGN(CONTAINER_SIZE_ALIGNMENT(size)); \
		} > NORMAL_RAM				\
		_f_data_load_##container_name = LOADADDR(.container_name.ram.data);

#define GEN_SECURE_CONTAINER_ROM_SECTION(container_name, size, ...) \
		.container_name.rom ALIGN(CONTAINER_SIZE_ALIGNMENT(size)) : { \
		_f_text_##container_name = .;		\
		*(.container_name.text)			\
		IF_ELSE(HAS_ARGS(__VA_ARGS__))(		\
			OBJS_TEXT(__VA_ARGS__)		\
		)()					\
		_e_text_##container_name = .;		\
		_f_rodata_##container_name = .; 	\
		*(.container_name.rodata)		\
		IF_ELSE(HAS_ARGS(__VA_ARGS__))(		\
			OBJS_RODATA(__VA_ARGS__)	\
		)()					\
		_e_rodata_##container_name = ALIGN(CONTAINER_SIZE_ALIGNMENT(size)); \
		} > SECURE_ROM

#define GEN_SECURE_CONTAINER_RAM_SECTION(container_name, size, ...) \
		.container_name.ram.data ALIGN(CONTAINER_SIZE_ALIGNMENT(size)) : { \
		_f_data_##container_name = .;		\
		*(.container_name.data)			\
		IF_ELSE(HAS_ARGS(__VA_ARGS__))(		\
			OBJS_DATA(__VA_ARGS__)		\
		)()					\
		_e_data_##container_name = .;		\
		} > SECURE_RAM AT > SECURE_ROM		\
		.container_name.ram.bss : {		\
		_f_bss_##container_name = .;		\
		*(.container_name.bss)			\
		IF_ELSE(HAS_ARGS(__VA_ARGS__))(		\
			OBJS_BSS(__VA_ARGS__)		\
		)()					\
		_e_bss_##container_name = ALIGN(CONTAINER_SIZE_ALIGNMENT(size)); \
		} > SECURE_RAM				\
		_f_data_load_##container_name = LOADADDR(.container_name.ram.data);

#elif SECURESHIELD_VERSION == 2

#define GEN_CONTAINER_ROM_SECTION(container_name, size, ...) \
		.container_name.rom ALIGN(CONTAINER_ADDRESS_ALIGNMENT) : { \
		_f_text_##container_name = .;		\
		*(.container_name.text)			\
		IF_ELSE(HAS_ARGS(__VA_ARGS__))(		\
			OBJS_TEXT(__VA_ARGS__)		\
		)()					\
		. = ALIGN(CONTAINER_ADDRESS_ALIGNMENT); \
		_e_text_##container_name = .;		\
		_f_rodata_##container_name = .; 	\
		*(.container_name.rodata)		\
		IF_ELSE(HAS_ARGS(__VA_ARGS__))(		\
			OBJS_RODATA(__VA_ARGS__)	\
		)()					\
		_e_rodata_##container_name = ALIGN(CONTAINER_ADDRESS_ALIGNMENT); \
		} > NORMAL_ROM

#define GEN_CONTAINER_RAM_SECTION(container_name, size, ...) \
		.container_name.ram.data ALIGN(CONTAINER_ADDRESS_ALIGNMENT) : { \
		_f_data_##container_name = .;		\
		*(.container_name.data)			\
		IF_ELSE(HAS_ARGS(__VA_ARGS__))(		\
			OBJS_DATA(__VA_ARGS__)		\
		)()					\
		_e_data_##container_name = .;		\
		} > NORMAL_RAM AT > NORMAL_ROM		\
		.container_name.ram.bss : {		\
		_f_bss_##container_name = .;		\
		*(.container_name.bss)			\
		IF_ELSE(HAS_ARGS(__VA_ARGS__))(		\
			OBJS_BSS(__VA_ARGS__)		\
		)()					\
		_e_bss_##container_name = ALIGN(CONTAINER_ADDRESS_ALIGNMENT); \
		} > NORMAL_RAM				\
		_f_data_load_##container_name = LOADADDR(.container_name.ram.data);

#define GEN_SECURE_CONTAINER_ROM_SECTION(container_name, size, ...) \
		.container_name.rom ALIGN(CONTAINER_ADDRESS_ALIGNMENT) : { \
		_f_text_##container_name = .;		\
		*(.container_name.text)			\
		IF_ELSE(HAS_ARGS(__VA_ARGS__))(		\
			OBJS_TEXT(__VA_ARGS__)		\
		)()					\
		. = ALIGN(CONTAINER_ADDRESS_ALIGNMENT); \
		_e_text_##container_name = .;		\
		_f_rodata_##container_name = .; 	\
		*(.container_name.rodata)		\
		IF_ELSE(HAS_ARGS(__VA_ARGS__))(		\
			OBJS_RODATA(__VA_ARGS__)	\
		)()					\
		_e_rodata_##container_name = ALIGN(CONTAINER_ADDRESS_ALIGNMENT); \
		} > SECURE_ROM

#define GEN_SECURE_CONTAINER_RAM_SECTION(container_name, size, ...) \
		.container_name.ram.data ALIGN(CONTAINER_ADDRESS_ALIGNMENT) : {	\
		_f_data_##container_name = .;		\
		*(.container_name.data)			\
		IF_ELSE(HAS_ARGS(__VA_ARGS__))(		\
			OBJS_DATA(__VA_ARGS__)		\
		)()					\
		_e_data_##container_name = .;		\
		} > SECURE_RAM AT > SECURE_ROM		\
		.container_name.ram.bss : {		\
		_f_bss_##container_name = .;		\
		*(.container_name.bss)			\
		IF_ELSE(HAS_ARGS(__VA_ARGS__))(		\
			OBJS_BSS(__VA_ARGS__)		\
		)()					\
		_e_bss_##container_name = ALIGN(CONTAINER_ADDRESS_ALIGNMENT); \
		} > SECURE_RAM				\
		_f_data_load_##container_name = LOADADDR(.container_name.ram.data);
#endif	/* SECURESHIELD_VERSION */

#endif /* __CONTAINER_CONFIG_H__ */
