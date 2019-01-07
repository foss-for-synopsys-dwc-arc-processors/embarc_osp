/* ------------------------------------------
 * Copyright (c) 2018, Synopsys, Inc. All rights reserved.

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
#ifndef _ARC_HAL_XY_H_
#define _ARC_HAL_XY_H_

#define U0	%r32
#define U1 	%r33
#define U2 	%r34

#ifdef ARC_FEATURE_XCCM_PRESENT
#define ARC_X_CCM __attribute__ ((section(".x_ccm")))
#else
#define ARC_X_CCM
#endif

#ifdef ARC_FEATURE_YCCM_PRESENT
#define ARC_Y_CCM __attribute__ ((section(".y_ccm")))
#else
#define ARC_Y_CCM
#endif

#define __expand_mod_name(reg_id) AUX_AGU_MOD ## reg_id
#define __expand_os_name(reg_id)  AUX_AGU_OS  ## reg_id
#define __expand_ap_name(reg_id)  AUX_AGU_AP  ## reg_id


#define _arc_agu_mod_set(agu_mod, ptr_id, data_type, addressing) \
			_arc_aux_write(__expand_mod_name(agu_mod), (uint32_t)(ptr_id | data_type | addressing))
#define _arc_agu_os_set(os_id, offset) \
			_arc_aux_write(__expand_os_name(os_id), (uint32_t)offset)
#define _arc_agu_ap_set(ap_id, addr) \
			_arc_aux_write(__expand_ap_name(ap_id), (uint32_t)addr)
#define _arc_agu_mod_get(mod_id) _arc_aux_read(__expand_mod_name(mod_id))
#define _arc_agu_os_get(os_id) _arc_aux_read(__expand_mod_name(os_id))
#define _arc_agu_ap_get(ap_id) _arc_aux_read(__expand_ap_name(ap_id))

#define __get_agu_mod(mod_id) _arc_aux_read(__expand_mod_name(mod_id))

#define _arc_agu_mod_vw(x) ((x) << 4)
#define _arc_agu_mod_fx(x) ((x) << 6)
#define _arc_agu_mod_dir(x) ((x) << 11)
#define _arc_agu_mod_rev (1 << 12)
#define _arc_agu_mod_repl (1 << 13)

#define _arc_agu_mod_sc(x) ((x) << 14)
#define _arc_agu_mod_offset_reg(x) ((x) << 25)
#define _arc_agu_mod_offset_imm(x) ((x) << 18)

#define _arc_agu_mod_wrap_reg(x) ((x) << 14)
#define _arc_agu_mod_wrap_imm(x) ((x) << 14)

#define _arc_agu_mod_opc(x) ((x) << 29)



#define ARC_AGU_DT_i32 (_arc_agu_mod_fx(11) | _arc_agu_mod_vw(2))
#define ARC_AGU_DT_i32_cast_i16 (_arc_agu_mod_fx(8) | _arc_agu_mod_vw(2))
#define ARC_AGU_DT_i32_cast_i8 (_arc_agu_mod_fx(4) | _arc_agu_mod_vw(2))

#define ARC_AGU_DT_i16 (_arc_agu_mod_fx(3) | _arc_agu_mod_vw(1))
#define ARC_AGU_DT_i16_cast_i8 (_arc_agu_mod_fx(0) | _arc_agu_mod_vw(1))

#define ARC_AGU_DT_i8 (_arc_agu_mod_fx(7) | _arc_agu_mod_vw(0))


#define ARC_AGU_DT_v2i16 (_arc_agu_mod_fx(3) | _arc_agu_mod_vw(2))
#define ARC_AGU_DT_v2i16_replic_i16 (_arc_agu_mod_fx(3) | _arc_agu_mod_vw(2) | _arc_agu_mod_repl)
#define ARC_AGU_DT_v2i16_cast_v2i8 (_arc_agu_mod_fx(0) | _arc_agu_mod_vw(2))

#define ARC_AGU_DT_v2i8 (_arc_agu_mod_fx(7) | _arc_agu_mod_vw(1))
#define ARC_AGU_DT_v2i8_replic_i8 (_arc_agu_mod_fx(7) | _arc_agu_mod_vw(1) | _arc_agu_mod_repl)

#define ARC_AGU_DT_v4i8 (_arc_agu_mod_fx(7) | _arc_agu_mod_vw(2))
#define ARC_AGU_DT_v4i8_replic_i8 (_arc_agu_mod_fx(7) | _arc_agu_mod_vw(2) | _arc_agu_mod_repl)

// AGU addressing modes with scling factor macroses

#define ARC_AGU_AD_INC_i32(offset) (_arc_agu_mod_opc(1) | _arc_agu_mod_offset_imm(offset) | _arc_agu_mod_sc(2))
#define ARC_AGU_AD_DEC_i32(offset) (_arc_agu_mod_opc(1) | _arc_agu_mod_offset_imm(offset) | _arc_agu_mod_sc(2) | __agu_mod_dir(1))
#define ARC_AGU_AD_INC_i32_REG(reg_id) (_arc_agu_mod_opc(0) | _arc_agu_mod_offset_reg(reg_id) | _arc_agu_mod_sc(2))
#define ARC_AGU_AD_DEC_i32_REG(reg_id) (_arc_agu_mod_opc(0) | _arc_agu_mod_offset_reg(reg_id) | _arc_agu_mod_sc(2) | __agu_mod_dir(1))

#define ARC_AGU_AD_BITREV_INC_i32(offset) (_arc_agu_mod_opc(3) | _arc_agu_mod_offset_imm(offset) | _arc_agu_mod_sc(2))
#define ARC_AGU_AD_BITREV_DEC_i32(offset) (_arc_agu_mod_opc(3) | _arc_agu_mod_offset_imm(offset) | _arc_agu_mod_sc(2) | __agu_mod_dir(1))
#define ARC_AGU_AD_BITREV_INC_i32_REG(reg_id) (_arc_agu_mod_opc(2) | _arc_agu_mod_offset_reg(reg_id) | _arc_agu_mod_sc(2))
#define ARC_AGU_AD_BITREV_DEC_i32_REG(reg_id) (_arc_agu_mod_opc(2) | _arc_agu_mod_offset_reg(reg_id) | _arc_agu_mod_sc(2) | __agu_mod_dir(1))

#define ARC_AGU_AD_INC_i16(offset) (_arc_agu_mod_opc(1) | _arc_agu_mod_offset_imm(offset) | _arc_agu_mod_sc(1))
#define ARC_AGU_AD_DEC_i16(offset) (_arc_agu_mod_opc(1) | _arc_agu_mod_offset_imm(offset) | _arc_agu_mod_sc(1) | __agu_mod_dir(1))
#define ARC_AGU_AD_INC_i16_REG(reg_id) (_arc_agu_mod_opc(0) | _arc_agu_mod_offset_reg(reg_id) | _arc_agu_mod_sc(1))
#define ARC_AGU_AD_DEC_i16_REG(reg_id) (_arc_agu_mod_opc(0) | _arc_agu_mod_offset_reg(reg_id) | _arc_agu_mod_sc(1) | __agu_mod_dir(1))

#define ARC_AGU_AD_BITREV_INC_i16(offset) (_arc_agu_mod_opc(3) | _arc_agu_mod_offset_imm(offset) | _arc_agu_mod_sc(1))
#define ARC_AGU_AD_BITREV_DEC_i16(offset) (_arc_agu_mod_opc(3) | _arc_agu_mod_offset_imm(offset) | _arc_agu_mod_sc(1) | __agu_mod_dir(1))
#define ARC_AGU_AD_BITREV_INC_i16_REG(reg_id) (_arc_agu_mod_opc(2) | _arc_agu_mod_offset_reg(reg_id) | _arc_agu_mod_sc(1))
#define ARC_AGU_AD_BITREV_DEC_i16_REG(reg_id) (_arc_agu_mod_opc(2) | _arc_agu_mod_offset_reg(reg_id) | _arc_agu_mod_sc(1) | __agu_mod_dir(1))

#define ARC_AGU_AD_INC_i8(offset) (_arc_agu_mod_opc(1) | _arc_agu_mod_offset_imm(offset) | _arc_agu_mod_sc(0))
#define ARC_AGU_AD_DEC_i8(offset) (_arc_agu_mod_opc(1) | _arc_agu_mod_offset_imm(offset) | _arc_agu_mod_sc(0) | __agu_mod_dir(1))
#define ARC_AGU_AD_INC_i8_REG(reg_id) (_arc_agu_mod_opc(0) | _arc_agu_mod_offset_reg(reg_id) | _arc_agu_mod_sc(0))
#define ARC_AGU_AD_DEC_i8_REG(reg_id) (_arc_agu_mod_opc(0) | _arc_agu_mod_offset_reg(reg_id) | _arc_agu_mod_sc(0) | __agu_mod_dir(1))

#define ARC_AGU_AD_BITREV_INC_i8(offset) (_arc_agu_mod_opc(3) | _arc_agu_mod_offset_imm(offset) | _arc_agu_mod_sc(0))
#define ARC_AGU_AD_BITREV_DEC_i8(offset) (_arc_agu_mod_opc(3) | _arc_agu_mod_offset_imm(offset) | _arc_agu_mod_sc(0) | __agu_mod_dir(1))
#define ARC_AGU_AD_BITREV_INC_i8_REG(reg_id) (_arc_agu_mod_opc(2) | _arc_agu_mod_offset_reg(reg_id) | _arc_agu_mod_sc(0))
#define ARC_AGU_AD_BITREV_DEC_i8_REG(reg_id) (_arc_agu_mod_opc(2) | _arc_agu_mod_offset_reg(reg_id) | _arc_agu_mod_sc(0) | __agu_mod_dir(1))

/******************************************************************************
 *
 *  AGU data types with type casting macroses for unsigned int data types
 *  unsigned int data type assumes lsb aligned data conversion without sign extension
 *
 ******************************************************************************/

#define ARC_AGU_DT_ui32 ARC_AGU_DT_i32
#define ARC_AGU_DT_ui32_cast_ui16 (_arc_agu_mod_fx(9) | _arc_agu_mod_vw(2))
#define ARC_AGU_DT_ui32_cast_ui8 (_arc_agu_mod_fx(5) | _arc_agu_mod_vw(2))

#define ARC_AGU_DT_ui16 ARC_AGU_DT_i16
#define ARC_AGU_DT_ui16_cast_ui8 (_arc_agu_mod_fx(1) | _arc_agu_mod_vw(1))

#define ARC_AGU_DT_ui8 ARC_AGU_DT_i8

#define ARC_AGU_DT_v2ui16 ARC_AGU_DT_v2i16
#define ARC_AGU_DT_v2ui16_replic_ui16 ARC_AGU_DT_v2i16_replic_i16
#define ARC_AGU_DT_v2ui16_cast_v2ui8 (_arc_agu_mod_fx(1) | _arc_agu_mod_vw(2))

#define ARC_AGU_DT_v2ui8 ARC_AGU_DT_v2i8
#define ARC_AGU_DT_v2ui8_replic_ui8 ARC_AGU_DT_v2i8_replic_i8

#define ARC_AGU_DT_v4ui8 ARC_AGU_DT_v4i8
#define ARC_AGU_DT_v4ui8_replic_ui8 ARC_AGU_DT_v4i8_replic_i8

// AGU addressing modes with scling factor macroses

#define ARC_AGU_AD_INC_ui32(offset) ARC_AGU_AD_INC_i32(offset)
#define ARC_AGU_AD_DEC_ui32(offset) ARC_AGU_AD_DEC_i32(offset)
#define ARC_AGU_AD_INC_ui32_REG(reg_id) ARC_AGU_AD_INC_i32_REG(reg_id)
#define ARC_AGU_AD_DEC_ui32_REG(reg_id) ARC_AGU_AD_DEC_i32_REG(reg_id)

#define ARC_AGU_AD_BITREV_INC_ui32(offset) ARC_AGU_AD_BITREV_INC_i32(offset)
#define ARC_AGU_AD_BITREV_DEC_ui32(offset) ARC_AGU_AD_BITREV_DEC_i32(offset)
#define ARC_AGU_AD_BITREV_INC_ui32_REG(reg_id) ARC_AGU_AD_BITREV_INC_i32_REG(reg_id)
#define ARC_AGU_AD_BITREV_DEC_ui32_REG(reg_id) ARC_AGU_AD_BITREV_DEC_i32_REG(reg_id)

#define ARC_AGU_AD_INC_ui16(offset) ARC_AGU_AD_INC_i16(offset)
#define ARC_AGU_AD_DEC_ui16(offset) ARC_AGU_AD_DEC_i16(offset)
#define ARC_AGU_AD_INC_ui16_REG(reg_id) ARC_AGU_AD_INC_i16_REG(reg_id)
#define ARC_AGU_AD_DEC_ui16_REG(reg_id) ARC_AGU_AD_DEC_i16_REG(reg_id)

#define ARC_AGU_AD_BITREV_INC_ui16(offset) ARC_AGU_AD_BITREV_INC_i16(offset)
#define ARC_AGU_AD_BITREV_DEC_ui16(offset) ARC_AGU_AD_BITREV_DEC_i16(offset)
#define ARC_AGU_AD_BITREV_INC_ui16_REG(reg_id) ARC_AGU_AD_BITREV_INC_i16_REG(reg_id)
#define ARC_AGU_AD_BITREV_DEC_ui16_REG(reg_id) ARC_AGU_AD_BITREV_DEC_i16_REG(reg_id)

#define ARC_AGU_AD_INC_ui8(offset) ARC_AGU_AD_INC_i8(offset)
#define ARC_AGU_AD_DEC_ui8(offset) ARC_AGU_AD_DEC_i8(offset)
#define ARC_AGU_AD_INC_ui8_REG(reg_id) ARC_AGU_AD_INC_i8_REG(reg_id)
#define ARC_AGU_AD_DEC_ui8_REG(reg_id) ARC_AGU_AD_DEC_i8_REG(reg_id)

#define ARC_AGU_AD_BITREV_INC_ui8(offset) ARC_AGU_AD_BITREV_INC_i8(offset)
#define ARC_AGU_AD_BITREV_DEC_ui8(offset) ARC_AGU_AD_BITREV_DEC_i8(offset)
#define ARC_AGU_AD_BITREV_INC_ui8_REG(reg_id) ARC_AGU_AD_BITREV_INC_i8_REG(reg_id)
#define ARC_AGU_AD_BITREV_DEC_ui8_REG(reg_id) ARC_AGU_AD_BITREV_DEC_i8_REG(reg_id)

/******************************************************************************
 *
 *  AGU data types with type casting macroses for q.xx data types
 *  q.xx data type assumes msb aligned data conversion
 *
 ******************************************************************************/

#define ARC_AGU_DT_q31 ARC_AGU_DT_i32
#define ARC_AGU_DT_q31_cast_q15 (_arc_agu_mod_fx(10) | _arc_agu_mod_vw(2))
#define ARC_AGU_DT_q31_cast_q7 (_arc_agu_mod_fx(6) | _arc_agu_mod_vw(2))

#define ARC_AGU_DT_q15 ARC_AGU_DT_i16
#define ARC_AGU_DT_q15_cast_q7 (_arc_agu_mod_fx(2) | _arc_agu_mod_vw(1))

#define ARC_AGU_DT_q7 ARC_AGU_DT_i8
#define ARC_AGU_DT_q7_cast_q15 (_arc_agu_mod_fx(2) | _arc_agu_mod_vw(1)) // packing

#define ARC_AGU_DT_v2q15 ARC_AGU_DT_v2i16
#define ARC_AGU_DT_v2q15_replic_q15 ARC_AGU_DT_v2i16_replic_i16
#define ARC_AGU_DT_v2q15_cast_v2q7 (_arc_agu_mod_fx(2) | _arc_agu_mod_vw(2))

#define ARC_AGU_DT_v2q7 ARC_AGU_DT_v2i8
#define ARC_AGU_DT_v2q7_replic_q7 ARC_AGU_DT_v2i8_replic_i8
#define ARC_AGU_DT_v2q7_cast_v2q15 (_arc_agu_mod_fx(2) | _arc_agu_mod_vw(2)) // packing

#define ARC_AGU_DT_v4q7 ARC_AGU_DT_v4i8
#define ARC_AGU_DT_v4q7_replic_q7 ARC_AGU_DT_v4i8_replic_i8

// AGU addressing modes with scling factor macroses for q.xx data types

#define ARC_AGU_AD_INC_q31(offset) ARC_AGU_AD_INC_i32(offset)
#define ARC_AGU_AD_DEC_q31(offset) ARC_AGU_AD_DEC_i32(offset)
#define ARC_AGU_AD_INC_q31_REG(reg_id) ARC_AGU_AD_INC_i32_REG(reg_id)
#define ARC_AGU_AD_DEC_q31_REG(reg_id) ARC_AGU_AD_DEC_i32_REG(reg_id)

#define ARC_AGU_AD_BITREV_INC_q31(offset) ARC_AGU_AD_BITREV_INC_i32(offset)
#define ARC_AGU_AD_BITREV_DEC_q31(offset) ARC_AGU_AD_BITREV_DEC_i32(offset)
#define ARC_AGU_AD_BITREV_INC_q31_REG(reg_id) ARC_AGU_AD_BITREV_INC_i32_REG(reg_id)
#define ARC_AGU_AD_BITREV_DEC_q31_REG(reg_id) ARC_AGU_AD_BITREV_DEC_i32_REG(reg_id)

#define ARC_AGU_AD_INC_q15(offset) ARC_AGU_AD_INC_i16(offset)
#define ARC_AGU_AD_DEC_q15(offset) ARC_AGU_AD_DEC_i16(offset)
#define ARC_AGU_AD_INC_q15_REG(reg_id) ARC_AGU_AD_INC_i16_REG(reg_id)
#define ARC_AGU_AD_DEC_q15_REG(reg_id) ARC_AGU_AD_DEC_i16_REG(reg_id)

#define ARC_AGU_AD_BITREV_INC_q15(offset) ARC_AGU_AD_BITREV_INC_i16(offset)
#define ARC_AGU_AD_BITREV_DEC_q15(offset) ARC_AGU_AD_BITREV_DEC_i16(offset)
#define ARC_AGU_AD_BITREV_INC_q15_REG(reg_id) ARC_AGU_AD_BITREV_INC_i16_REG(reg_id)
#define ARC_AGU_AD_BITREV_DEC_q15_REG(reg_id) ARC_AGU_AD_BITREV_DEC_i16_REG(reg_id)

#define ARC_AGU_AD_INC_q7(offset) ARC_AGU_AD_INC_i8(offset)
#define ARC_AGU_AD_DEC_q7(offset) ARC_AGU_AD_DEC_i8(offset)
#define ARC_AGU_AD_INC_q7_REG(reg_id) ARC_AGU_AD_INC_i8_REG(reg_id)
#define ARC_AGU_AD_DEC_q7_REG(reg_id) ARC_AGU_AD_DEC_i8_REG(reg_id)

#define ARC_AGU_AD_BITREV_INC_q7(offset) ARC_AGU_AD_BITREV_INC_i8(offset)
#define ARC_AGU_AD_BITREV_DEC_q7(offset) ARC_AGU_AD_BITREV_DEC_i8(offset)
#define ARC_AGU_AD_BITREV_INC_q7_REG(reg_id) ARC_AGU_AD_BITREV_INC_i8_REG(reg_id)
#define ARC_AGU_AD_BITREV_DEC_q7_REG(reg_id) ARC_AGU_AD_BITREV_DEC_i8_REG(reg_id)

// AGU modificator for reverse vector elements (can be useful for Endian conversions)

#define ARC_AGU_DT_vector_reverse __agu_mod_rev

// AGU wrapping modifiers

#define ARC_AGU_AD_WRAP_INC_RR(inc_os_id, modulo_os_id) (_arc_agu_mod_opc(4) | _arc_agu_mod_offset_reg(inc_os_id) | __agu_mod_wrap_reg(modulo_os_id))
#define ARC_AGU_AD_WRAP_DEC_RR(dec_os_id, modulo_os_id) (_arc_agu_mod_opc(4) | _arc_agu_mod_offset_reg(dec_os_id) | __agu_mod_wrap_reg(modulo_os_id) | __agu_mod_dir(1))
#define ARC_AGU_AD_WRAP_INC_II(offset, modulo) (_arc_agu_mod_opc(5) | _arc_agu_mod_offset_imm(offset) | __agu_mod_wrap_imm(modulo))
#define ARC_AGU_AD_WRAP_DEC_II(offset, modulo) (_arc_agu_mod_opc(5) | _arc_agu_mod_offset_imm(offset) | __agu_mod_wrap_imm(modulo) | __agu_mod_dir(1))
#define ARC_AGU_AD_WRAP_INC_RI(dec_os_id, modulo) (_arc_agu_mod_opc(6) | _arc_agu_mod_offset_reg(dec_os_id) | __agu_mod_wrap_imm(modulo))
#define ARC_AGU_AD_WRAP_DEC_RI(dec_os_id, modulo) (_arc_agu_mod_opc(6) | _arc_agu_mod_offset_reg(dec_os_id) | __agu_mod_wrap_imm(modulo) | __agu_mod_dir(1))

// Immediate Wrapping constants

#define AGU_WRAP_IMM_2 0
#define AGU_WRAP_IMM_4 1
#define AGU_WRAP_IMM_8 2
#define AGU_WRAP_IMM_16 3
#define AGU_WRAP_IMM_32 4
#define AGU_WRAP_IMM_64 5
#define AGU_WRAP_IMM_128 6
#define AGU_WRAP_IMM_256 7
#define AGU_WRAP_IMM_512 8
#define AGU_WRAP_IMM_1024 9
#define AGU_WRAP_IMM_2048 10
#define AGU_WRAP_IMM_4096 11
#define AGU_WRAP_IMM_8192 12
#define AGU_WRAP_IMM_16384 13
#define AGU_WRAP_IMM_32768 14
#define AGU_WRAP_IMM_65536 15

// push pop macros to follow calling conventions

#define _arc_agu_store_regs(ap_num, mod_num, os_num) \
 int32_t agu_temp_idx; \
 uint32_t agu_temp_ap[ap_num]; \
 uint32_t agu_temp_mod[mod_num]; \
 uint32_t agu_temp_os[os_num]; \
 int32_t agu_temp_ap_num = ap_num; \
 int32_t agu_temp_mod_num = mod_num; \
 int32_t agu_temp_os_num = os_num; \
 for (agu_temp_idx = 0; agu_temp_idx < ap_num; agu_temp_idx++) agu_temp_ap[agu_temp_idx] = _lr(AGU_AUX_AP0 + agu_temp_idx); \
 for (agu_temp_idx = 0; agu_temp_idx < mod_num; agu_temp_idx++) agu_temp_mod[agu_temp_idx] = _lr(AGU_AUX_MOD0 + agu_temp_idx); \
 for (agu_temp_idx = 0; agu_temp_idx < os_num; agu_temp_idx++) agu_temp_os[agu_temp_idx] = _lr(AGU_AUX_OS0 + agu_temp_idx); {}

#define _arc_agu_restore_regs() \
 for (agu_temp_idx = 0; agu_temp_idx < agu_temp_ap_num; agu_temp_idx++) _sr(agu_temp_ap[agu_temp_idx], AGU_AUX_AP0 + agu_temp_idx); \
 for (agu_temp_idx = 0; agu_temp_idx < agu_temp_mod_num; agu_temp_idx++) _sr(agu_temp_mod[agu_temp_idx], AGU_AUX_MOD0 + agu_temp_idx); \
 for (agu_temp_idx = 0; agu_temp_idx < agu_temp_os_num; agu_temp_idx++) _sr(agu_temp_os[agu_temp_idx], AGU_AUX_OS0 + agu_temp_idx); {}

#endif /* _ARC_HAL_XY_H_ */
