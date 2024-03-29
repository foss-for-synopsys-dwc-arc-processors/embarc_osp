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
 * @file
 * @ingroup ARC_HAL_EXCEPTION_CPU ARC_HAL_EXCEPTION_INTERRUPT
 * @brief header file of exception and interrupt management module
 */

#ifndef _ARC_HAL_EXCEPTION_H_
#define _ARC_HAL_EXCEPTION_H_

#include "embARC_toolchain.h"
#include "arc/arc.h"
#include "arc/arc_builtin.h"

#ifndef __ASSEMBLY__
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ARC_HAL_EXCEPTION_CPU
 * @{
 */
#ifndef NUM_EXC_CPU
/*!< number of CPU exceptions */
#define NUM_EXC_CPU             16
#endif

#ifndef NUM_EXC_INT
/*!< number of interrupt exceptions, defined in arc_feature_config.h */
#define NUM_EXC_INT             9
#endif

/*!< total number of exceptions */
#define NUM_EXC_ALL             (NUM_EXC_CPU + NUM_EXC_INT)

#ifdef ARC_FEATURE_SEC_PRESENT
typedef struct int_exc_frame {
	uint32_t erbta;

	uint32_t r30;   /* r30 is useless, skipped? */
	uint32_t ilink; /* r29 is useless, skipped?*/
	/* r28 is sp, saved other place */
	uint32_t fp;    /* r27 */
	uint32_t gp;    /* r26 */

#if ARC_FEATURE_DSP || ARC_FEATURE_FPU || ARC_FEATURE_MPU_OPTION_NUM > 6
	/* accl and acch, common for mpy_option >6 and fpu_fma option */
	uint32_t r59;
	uint32_t r58;
#endif

	uint32_t r12;

	uint32_t lp_end, lp_start, lp_count;

#if ARC_FEATURE_CODE_DENSITY
	uint32_t ei, ldi, jli;
#endif

	uint32_t r0, r1, r2, r3;
#ifndef ARC_FEATURE_RF16
	uint32_t r4, r5, r6, r7, r8, r9;
#endif
	uint32_t r10, r11;

	uint32_t blink; /* r31 */
	uint32_t ret;
	uint32_t sec_stat;
	uint32_t status32;
} EMBARC_PACKED INT_EXC_FRAME_T;
#else
typedef struct int_exc_frame {
	uint32_t erbta;

	uint32_t r30;   /* r30 is useless, skipped? */
	uint32_t ilink; /* r29 is useless, skipped?*/
	/* r28 is sp, saved other place */
	uint32_t fp;    /* r27 */
	uint32_t gp;    /* r26 */

#if ARC_FEATURE_DSP || ARC_FEATURE_FPU || ARC_FEATURE_MPU_OPTION_NUM > 6
	/* accl and acch, common for mpy_option >6 and fpu_fma option */
	uint32_t r59;
	uint32_t r58;
#endif
	uint32_t r12;

	uint32_t r0, r1, r2, r3;
#ifndef ARC_FEATURE_RF16
	uint32_t r4, r5, r6, r7, r8, r9;
#endif
	uint32_t r10, r11;

	uint32_t blink; /* r31 */

	uint32_t lp_end, lp_start, lp_count;

#if ARC_FEATURE_CODE_DENSITY
	uint32_t ei, ldi, jli;
#endif

	uint32_t ret;
	uint32_t status32;
} EMBARC_PACKED INT_EXC_FRAME_T;
#endif

typedef struct dsp_ext_frame {
	/*  todo xy memory support */
#if ARC_FEATURE_DSP_COMPLEX
	uint32_t dsp_fft_ctrl;
	uint32_t dsp_bfly0;
#endif
	uint32_t acc0_ghi;
	uint32_t acc0_hi;
	uint32_t acc0_glo;
	uint32_t acc0_lo;
	uint32_t dsp_ctrl;

} EMBARC_PACKED DSP_EXT_FRAME_T;

typedef struct fpu_ext_frame {
#if ARC_FEATURE_FPU_DA
	uint32_t dpfp2h;
	uint32_t dpfp2l;
	uint32_t dpfp1h;
	uint32_t dpfp1l;
#endif

	uint32_t fpu_status;
	uint32_t fpu_ctrl;

} EMBARC_PACKED FPU_EXT_FRAME_T;

typedef struct callee_frame {

#if ARC_FEATURE_FPU_DSP_CONTEXT

#if ARC_FEATURE_DSP
	DSP_EXT_FRAME_T dsp_regs;
#endif

#if ARC_FEATURE_FPU
	FPU_EXT_FRAME_T fpu_ext_regs;
#endif

#endif /* ARC_FEATURE_FPU_DSP_CONTEXT */

#if defined(ARC_ENABLE_EXTRA_CALLEE)
#if ARC_FEATURE_SEC_PRESENT
	uint32_t secure_kernel_sp;
	uint32_t kernel_sp;
#else
	uint32_t user_sp;
#endif
#endif

#ifndef ARC_FEATURE_RF16
	uint32_t r25;
	uint32_t r24;
	uint32_t r23;
	uint32_t r22;
	uint32_t r21;
	uint32_t r20;
	uint32_t r19;
	uint32_t r18;
	uint32_t r17;
	uint32_t r16;
#endif
	uint32_t r15;
	uint32_t r14;
	uint32_t r13;
} EMBARC_PACKED CALLEE_FRAME_T;

typedef struct processor_frame {
	CALLEE_FRAME_T callee_regs;
	INT_EXC_FRAME_T exc_frame;
} EMBARC_PACKED PROCESSOR_FRAME_T;

#define ARC_PROCESSOR_FRAME_T_SIZE        (sizeof(PROCESSOR_FRAME_T) / sizeof(uint32_t))
#define ARC_EXC_FRAME_T_SIZE              (sizeof(INT_EXC_FRAME_T) / sizeof(uint32_t))
#define ARC_CALLEE_FRAME_T_SIZE           (sizeof(CALLEE_FRAME_T) / sizeof(uint32_t))

#ifdef OS_FREERTOS
extern uint32_t exc_nest_count;
#endif

/**
 * @brief write the exception vector base
 *
 * @param[in] vec_base the target vector base
 */
Inline void arc_vector_base_write(const void *vec_base)
{
	arc_aux_write(AUX_INT_VECT_BASE, (uint32_t)vec_base);
}

/**
 * @brief read current exception vector base
 *
 * @returns exception vector base (uint32_t)
 */
Inline uint32_t arc_vector_base_read(void)
{
	return arc_aux_read(AUX_INT_VECT_BASE);
}

#ifdef OS_FREERTOS
/**
 * @brief  sense whether in exc/interrupt processing
 *
 * @retval 0	not in exc/interrupt processing
 * @retval 1	in exc/interrupt processing
 */
Inline uint32_t exc_sense(void)
{
	return (exc_nest_count > 0U);
}
#endif

/** @}*/

/**
 * @addtogroup ARC_HAL_EXCEPTION_INTERRUPT
 * @{
 */
#ifndef INT_PRI_MIN
#define INT_PRI_MIN     (-2)            /*!< the minimum interrupt priority */
#endif

#define INT_PRI_MAX (-1)                /*!< the maximum interrupt priority */
/**
 * @brief disable the specific interrupt
 *
 * @param[in] intno interrupt number
 */
Inline void arc_int_disable(const uint32_t intno)
{
	arc_aux_write(AUX_IRQ_SELECT, intno);
	arc_aux_write(AUX_IRQ_ENABLE, 0);
}

/**
 * @brief  enable the specific int
 *
 * @param[in] intno interrupt number
 */
Inline void arc_int_enable(const uint32_t intno)
{
	arc_aux_write(AUX_IRQ_SELECT, intno);
	arc_aux_write(AUX_IRQ_ENABLE, 1);
}

/**
 * @brief  check whether the specific int is enabled
 *
 * @param[in] intno interrupt number
 * @return 0 disabled, 1 enabled
 */
Inline uint32_t arc_int_enabled(const uint32_t intno)
{
	arc_aux_write(AUX_IRQ_SELECT, intno);
	return arc_aux_read(AUX_IRQ_ENABLE);
}

/**
 * @brief  get the interrupt priority mask
 *
 * @returns interrupt priority mask, negative num
 */
Inline uint32_t arc_int_ipm_get(void)
{
	return ((arc_aux_read(AUX_STATUS32) >> 1) & 0x0f);
}

/**
 * @brief  set the interrupt priority mask
 *
 * @param[in] intpri interrupt priority
 */
Inline void arc_int_ipm_set(uint32_t intpri)
{
	volatile uint32_t status;

	status = arc_aux_read(AUX_STATUS32) & ~0x1e;

	status = status | ((intpri << 1) & 0x1e);
	/* sr cannot write AUX_STATUS32 */
	Asm("kflag %0" ::"ir" (status));
}

/**
 * @brief  get current interrupt priority mask
 *
 * @param[in] intno interrupt number
 */
Inline uint32_t arc_int_pri_get(const uint32_t intno)
{
	arc_aux_write(AUX_IRQ_SELECT, intno);
	return arc_aux_read(AUX_IRQ_PRIORITY) & 0xf;
}

/**
 * @brief set interrupt priority
 *
 * @param[in] intno interrupt number
 * @param[in] intpri interrupt priority
 */
Inline void arc_int_pri_set(const uint32_t intno, uint32_t intpri)
{
	arc_aux_write(AUX_IRQ_SELECT, intno);
	arc_aux_write(AUX_IRQ_PRIORITY, intpri | (arc_aux_read(AUX_IRQ_PRIORITY) & 0xfffffff0));
}

/**
 * @brief set interrupt secure or not secure
 *
 * @param[in] intno interrupt number
 * @param[in] secure 0 for normal, >0 for secure
 */
Inline void arc_int_secure_set(const uint32_t intno, uint32_t secure)
{

	arc_aux_write(AUX_IRQ_SELECT, intno);

	if (secure) {
		arc_aux_write(AUX_IRQ_PRIORITY, arc_aux_read(AUX_IRQ_PRIORITY) |
			      (1 << AUX_IRQ_PRIORITY_BIT_S));
	} else {
		arc_aux_write(AUX_IRQ_PRIORITY, arc_aux_read(AUX_IRQ_PRIORITY) & 0xf);
	}

}

/**
 * @brief  probe the pending status of interrupt
 *
 * @param[in] intno interrupt number
 *
 * @returns 1 pending, 0 no pending
 */
Inline uint32_t arc_int_probe(const uint32_t intno)
{
	arc_aux_write(AUX_IRQ_SELECT, intno);
	return arc_aux_read(AUX_IRQ_PENDING);
}

/**
 * @brief  trigger the interrupt in software
 *
 * @param[in] intno interrupt number
 */
Inline void arc_int_sw_trigger(const uint32_t intno)
{
	arc_aux_write(AUX_IRQ_HINT, intno);
}

/**
 * @brief  config the interrupt level triggered or pulse triggered
 *
 * @param[in] intno interrupt number
 * @param[in] level 0-level trigger, 1-pluse triggered
 */
Inline void arc_int_level_config(const uint32_t intno, const uint32_t level)
{
	arc_aux_write(AUX_IRQ_SELECT, intno);
	arc_aux_write(AUX_IRQ_TRIGGER, level);
}

/**
 * @brief  lock cpu, disable interrupts
 */
Inline void arc_lock(void)
{
	arc_clri();
}

/**
 * @brief  unlock cpu, enable interrupts to happen
 */
Inline void arc_unlock(void)
{
	arc_seti(0);
}

/**
 * @brief  cpu is locked?
 *
 * @returns 1 locked, 0 unlocked
 */
Inline uint32_t arc_locked(void)
{
	if (arc_aux_read(AUX_STATUS32) & AUX_STATUS_MASK_IE) {
		return 0;
	} else {
		return 1;
	}
}

/**
 * @brief  lock cpu and status
 *
 * @returns cpu status
 */
Inline uint32_t arc_lock_save(void)
{
	return arc_clri();
}

/**
 * @brief  unlock cpu with the specific status
 *
 * @param[in] status  cpu status saved by cpu_lock_save
 */
Inline void arc_unlock_restore(const uint32_t status)
{
	arc_seti(status);
}

/**
 * @brief  interrupt is active ?
 *
 * @returns 1 active, 0 inactive
 */
Inline uint32_t arc_int_active(void)
{
	if (arc_aux_read(AUX_IRQ_ACT) == 0) {
		return 0;
	} else {
		return 1;
	}
}
/** @}*/

/**
 * @addtogroup ARC_HAL_EXCEPTION_CPU
 * @{
 */
/**
 * @typedef EXC_ENTRY_T
 * @brief  the data type for exception entry
 */
typedef void (*EXC_ENTRY_T) (void);
/**
 * @typedef EXC_HANDLER_T
 * @brief  the data type for exception handler
 */
typedef void (*EXC_HANDLER_T) (void *exc_frame);
/** @}*/

/**
 * @ingroup ARC_HAL_EXCEPTION_INTERRUPT
 * @typedef INT_HANDLER_T
 * @brief  the data type for interrupt handler
 */
typedef void (*INT_HANDLER_T) (void *ptr);

extern EXC_ENTRY_T exc_entry_table[NUM_EXC_ALL];
extern EXC_HANDLER_T exc_int_handler_table[NUM_EXC_ALL];

/** @ingroup ARC_HAL_EXCEPTION_CPU
 * @{
 */
/**
 * @fn _arc_reset
 * @brief the reset entry
 */
extern void _arc_reset(void);
/**
 * @fn exc_entry_cpu
 * @brief the default CPU exception entry
 */
extern void exc_entry_cpu(void);

/**
 * @fn exc_entry_firq
 * @brief the fast interrupt exception entry
 */
extern void exc_entry_firq(void);
/**
 * @fn exc_entry_int
 * @brief the interrupt exception entry
 */
extern void exc_entry_int(void);
/** @}*/

/* exception related APIs */
extern void exc_int_init(void);
extern int32_t exc_entry_install(const uint32_t excno, EXC_ENTRY_T entry);
extern EXC_ENTRY_T exc_entry_get(const uint32_t excno);
extern int32_t exc_handler_install(const uint32_t excno, EXC_HANDLER_T handler);
extern EXC_HANDLER_T exc_handler_get(const uint32_t excno);

/* interrupt related APIs */
extern int32_t int_disable(const uint32_t intno);
extern int32_t int_enable(const uint32_t intno);
/**
 * @brief  check whether the specific int is enabled
 *
 * @param[in] intno interrupt number
 * @return 0 disabled, 1 enabled, < 0 error
 */
extern int32_t int_enabled(const uint32_t intno);
extern int32_t int_ipm_get(void);
extern int32_t int_ipm_set(int32_t intpri);

/**
 * @brief  get current interrupt priority mask
 *
 * @param[in] intno interrupt number
 * @return  <0 interrupt priority, 0 error
 */
extern int32_t int_pri_get(const uint32_t intno);
extern int32_t int_pri_set(const uint32_t intno, int32_t intpri);

/**
 * @brief  probe the pending status of interrupt
 *
 * @param[in] intno interrupt number
 * @returns 1 pending, 0 no pending, -1 error
 */
extern int32_t int_probe(const uint32_t intno);

/**
 * @brief  trigger the interrupt in software
 *
 * @param[in] intno interrupt number
 * @return 0 ok, -1 error
 */
extern int32_t int_sw_trigger(const uint32_t intno);

/**
 * @brief  config the interrupt level triggered or pulse triggered
 *
 * @param[in] intno interrupt number
 * @param[in] level 0-level trigger, 1-pluse triggered
 * @return 0 ok, -1 error
 */
extern int32_t int_level_config(const uint32_t intno, const uint32_t level);
extern void cpu_lock(void);
extern void cpu_unlock(void);
extern uint32_t cpu_lock_save(void);
extern void cpu_unlock_restore(const uint32_t status);
extern int32_t int_handler_install(const uint32_t intno, INT_HANDLER_T handler);

/**
 * @ingroup ARC_HAL_EXCEPTION_INTERRUPT
 * @brief  get the installed an interrupt handler
 * @param[in] intno interrupt number
 * @return the installed interrupt handler or NULL
 */
extern INT_HANDLER_T int_handler_get(const uint32_t intno);
extern int32_t int_secure_set(const uint32_t intno, uint32_t secure);
extern void arc_firq_stack_set(uint8_t *firq_sp);

#ifdef __cplusplus
}
#endif
#endif  /* __ASSEMBLY__ */

#if defined(LIB_SECURESHIELD) && defined(LIB_SECURESHIELD_OVERRIDES) && (SECURESHIELD_VERSION == 1)
#define OVERRIDE_ARC_HAL_EXCEPTION_H
#include "secureshield_overrides.h"
#undef OVERRIDE_ARC_HAL_EXCEPTION_H
#endif

#endif /* _ARC_HAL_EXCEPTION_H_*/
