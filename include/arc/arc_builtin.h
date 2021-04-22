/* ------------------------------------------
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

/**
 * \file
 * \ingroup ARC_HAL_BUILTIN
 * \brief header file of builtin and helper functions
 *
 * The Metaware toolchain and the GNU toolchain are supported. The details please go to see the file.
 */

/**
 * \addtogroup	ARC_HAL_BUILTIN
 * @{
 */

#ifndef _ARC_HAL_BUILTIN_H_
#define _ARC_HAL_BUILTIN_H_

#include "embARC_toolchain.h"
#include "arc/arc.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined (__MW__)	/* Metaware toolchain */

/*
 * !< arc_compiler_usually (expr) evaluates expression expr and
 * informs the compiler that the value is usually true.
 */
#define arc_compiler_usually(a)			_Usually((a))

/*
 * !< arc_compiler_rarely (expr) evaluates expression expr and
 * informs the compiler that the value is rarely true.
 */
#define arc_compiler_rarely(a)			_Rarely((a))

#elif defined (__GNU__) /* GNU toolchain */

/**
 * @brief arc_compiler_usually(expr) evaluates expression expr and
 * 		  informs the compiler that the value is usually true.
 */
#define arc_compiler_usually(a)         __builtin_expect((int32_t)(a), 1)

/**
 * @brief arc_compiler_rarely(expr) evaluates expression expr and
 *	informs the compiler that the value is rarely true.
 */
#define arc_compiler_rarely(a)          __builtin_expect((int32_t)(a), 0)

#endif

/**
 * @brief Read auxiliary register
 *
 * @param aux auxiliary register address
 * @return value of auxiliary register
 */
Inline uint32_t arc_aux_read(uint32_t aux)
{
	uint32_t ret;

	Asm("lr %0, [%1]"
		: "=r" (ret)
		: "r" (aux));

	return ret;
}

/**
 * @brief Write auxiliary register
 *
 * @param aux auxiliary register address
 * @param val Value to write
 */
Inline void arc_aux_write(uint32_t aux, uint32_t val)
{
	Asm(
		"sr   %0, [%1]	\n"
		:
		: "ir" (val), "r" (aux));
}

/**
 * @brief Call brk instruction
 * stop the core through a brk instruction
 */
Inline void arc_brk(void)
{
	Asm("brk");
}

/**
 * @brief Call sync instruction
 *
 */
Inline void arc_sync(void)
{
	Asm("sync");
}

/**
 * @brief Call kflag instruction to change status32
 *
 * @param flag Flag to set in status32
 */
Inline void arc_kflag(uint32_t flag)
{
	/*sr cannot write AUX_STATUS32 */
	Asm("kflag %0" ::"ir" (flag));
}

/**
 * @brief Call nop_s function
 * flush the pipeline by nop_s instruction
 */
Inline void arc_nop(void)
{
	Asm("nop_s");
}

/**
 * @brief Call clri instruction
 * call a clri instruction to disable interrupt
 * @return interrupt related bits in status32
 */
Inline uint32_t arc_clri(void)
{
	uint32_t v;

	Asm("clri %0" : "=r" (v):: "memory");
	return v;
}

/**
 * @brief Call seti instruction
 * call a set instruction to change interrupt status
 * @param key  interrupt status
 */
Inline void arc_seti(uint32_t key)
{
	Asm("seti %0" : : "ir" (key) : "memory");
}

/**
 * @brief Swap bytes order of a 32-bits value
 *
 * @param val Target value
 * @return Swapped value
 */
Inline uint32_t arc_swap32(uint32_t val)
{
	uint32_t v;

	Asm("swape %0, %1" : "=r" (v) : "r" (val));
	return v;
}

/**
 * @brief Swap bytes order of a 32-bits value and return high 16-bits
 *
 * @param val Target value
 * @return High 16 bits of the swapped value
 */
Inline uint16_t arc_swap16(uint32_t val)
{
	uint32_t temp;
	uint32_t v;

	Asm("swape %0, %1" : "=r" (temp) : "r" (val));
	Asm("lsr16 %0, %1" : "=r" (v) : "r" (temp));
	return (uint16_t)v;
}

/**
 * @name cache related helper function
 * @{
 */

/**
 * @brief  Read memory and bypass the cache
 * @param[in] ptr Memory address
 * @return Date in the memory
 */
Inline uint32_t arc_read_uncached_32(void *ptr)
{
	uint32_t ret;

	Asm("ld.di %0, [%1]" : "=r" (ret) : "r" (ptr));
	return ret;
}

/**
 * @brief  Write memory and bypass the cache
 * @param[in] ptr memory address
 * @param[in] data  vaule to be written
 */
Inline void arc_write_uncached_32(void *ptr, uint32_t data)
{
	Asm("st.di %0, [%1]" :: "r" (data), "r" (ptr));
}

/**
 * @brief  Read memory with cache
 * @param[in] ptr Memory address
 * @returns Date in the memory
 */
Inline uint32_t arc_read_cached_32(void *ptr)
{
	uint32_t ret;

	Asm("ld %0, [%1]" : "=r" (ret) : "r" (ptr));
	return ret;
}

/**
 * @brief  Read memory with cache
 * @param[in] ptr Memory address
 * @param[in] data Data to be written
 * @return Description
 */
Inline void arc_write_cached_32(void *ptr, uint32_t data)
{
	Asm("st %0, [%1]" :: "r" (data), "r" (ptr));
}

/**
 * @brief Go to main function with proper arguments
 * @param  argc Argument count
 * @param  argv Argument content array
 * @retval Return value of main function
 */
Inline int32_t arc_goto_main(int32_t argc, char **argv)
{
	int32_t ret;

	Asm(
		"mov %%r0, %1\n"
		"mov %%r1, %2\n"
		"push_s %%blink\n"
		"jl main\n"
		"pop_s %%blink\n"
		"mov %0, %%r0"
		: "=r" (ret) : "r" (argc), "r" (argv));
	return (int32_t)ret;
}

/**
 *
 * @brief Find most significant bit set in a 32-bit word
 *
 * This routine finds the first bit set starting from the most significant bit
 * in the argument passed in and returns the index of that bit. Bits are
 * numbered starting at 1 from the least significant bit. A return value of
 * zero indicates that the value passed is zero.
 *
 * @return Most significant bit set, 0 if @a val is 0
 */
Inline uint32_t arc_find_msb(uint32_t val)
{
	uint32_t bit;

	Asm(
		/* BITSCAN_OPTION is required */
		"fls.f %0, %1;\n"
		"add.nz %0, %0, 1;\n"
		: "=r" (bit)
		: "r" (val));

	return bit;
}

/**
 *
 * @brief Find least significant bit set in a 32-bit word
 *
 * This routine finds the first bit set starting from the least significant bit
 * in the argument passed in and returns the index of that bit.  Bits are
 * numbered starting at 1 from the least significant bit.  A return value of
 * zero indicates that the value passed is zero.
 *
 * @return Least significant bit set, 0 if @a val is 0
 */
Inline uint32_t arc_find_lsb(uint32_t val)
{
	uint32_t bit;

	Asm(
		/* BITSCAN_OPTION is required */
		"ffs.f %0, %1;\n"
		"add.nz %0, %0, 1;\n"
		"mov.z %0, 0;\n"
		: "=&r" (bit)
		: "r" (val));

	return bit;
}

/**
 * @brief Read core id
 *
 * @return Core id
 */
Inline uint32_t arc_core_id(void)
{
	uint32_t ret;

	Asm(
		"lr %0, [%1]\n"
		"xbfu %0, %0, 0xe8\n"
		: "=r" (ret)
		: "i" (AUX_IDENTITY));

	return ret;
}

#ifdef __cplusplus
}
#endif

#if defined(LIB_SECURESHIELD) && defined(LIB_SECURESHIELD_OVERRIDES) && (SECURESHIELD_VERSION == 1)
#define OVERRIDE_ARC_HAL_BUILTIN_H
#include "secureshield_overrides.h"
#endif

/** @} */
#endif /* _ARC_HAL_BUILTIN_H_ */
/** @} */